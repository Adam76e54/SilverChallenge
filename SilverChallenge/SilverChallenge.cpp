#include <Arduino_FreeRTOS.h>

#include "TCRT5000.h"
#include "NetworkSetup.h"
#include "HCSR04.h"
#include "L293D.h"
#include "ROB12629.h"
#include "GUI.h"
#include "Buffer.h"
#include "StateVariables.h"
#include "Commands.h"
#include "CD4021.h"

// Declaration of Interrupt Service Routines
void leftISR(); 
ROB12629 leftEncoder(2);

// Driver is global
L293D driver(6,7,11,12,9,10);

// This needs to be shared too
Buffer<200> outBuffer;

// Tasks (for scheduler to handle)
void telemetry(void *);
void sense(void *);

// Declare handles for each task, used to pass to task-analysing functions like uxTaskGetStackHighWaterMark()
TaskHandle_t telemetryHandle, senseHandle;

// Set up semaphores (I think we just need a mutex?)
SemaphoreHandle_t stateSemaphore, bufferSemaphore;

void setup() {
  // Set up serial for debugging
  Serial.begin(115200);

  // Set up network
  wifi::initialiseAccessPoint();

  // Set up semaphore


  // Set up tasks
  xTaskCreate(telemetry, "Read from GUI", 2048, nullptr, 1, &telemetryHandle);
  xTaskCreate(sense, "Sense and drive", 2048, nullptr, 2, &senseHandle);

  stateSemaphore = xSemaphoreCreateBinary();
  configASSERT(stateSemaphore != nullptr);
  xSemaphoreGive(stateSemaphore);  // initialise to 'unlocked'

  bufferSemaphore = xSemaphoreCreateBinary();
  configASSERT(bufferSemaphore != nullptr);
  xSemaphoreGive(bufferSemaphore);  // initialise to 'unlocked'
  // Start scheduler (does not return)
  vTaskStartScheduler();
}

void loop() {
  // This is the idle task if everything else is blocking, everything else is handled by the scheduler
}

// - ISRs -
void leftISR(){
  leftEncoder.increment();
}

// - TASK 1 -
void telemetry(void *parameters){
  // Networking objects
  Buffer<200> inBuffer;
  WiFiServer server(wifi::PORT);
  server.begin();

  WiFiClient GUI;

  while(true){
    // Reconnect if disconnected
    keep(GUI, server);
    // Read a command if there is one
    read(GUI, inBuffer);
    // Handle a command if there is one
    handle(inBuffer, driver);

    // We've only got one thing to send right now
    sendDistance(GUI, state::currentDistance);

    // Don't repeat this task again for at least 300 ms
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

// - TASK 2 -
void sense(void *parameters){
  // Declare physical constants
  constexpr float CIRCUMFERENCE = 20.4, DIAMETER = 6.5, AXLE = 14.5;

  // Hardware objects
  HCSR04 ears(8, 3); // PINS NEED TO BE CHANGED
  CD4021 shifter(4, 5, 13);

  // Set up hardware
  driver.begin();
  ears.begin();
  shifter.begin();

  // We pass ISRs to the encoders
  leftEncoder.begin(leftISR);

  constexpr TickType_t period= pdMS_TO_TICKS(15);
  TickType_t lastWakeTime = xTaskGetTickCount();
  while(true){
    xSemaphoreTake(stateSemaphore, portMAX_DELAY);//need to change that timeout

    driver.forward(0.5, 0.5);

    xSemaphoreGive(stateSemaphore);
    xTaskDelayUntil(&lastWakeTime, period); 
  }
}

// - OTHER FUNCTIONS -
