#include <Arduino_FreeRTOS.h>

#include "NetworkSetup.h"
#include "HCSR04.h"
#include "L293D.h"
#include "ROB12629.h"
#include <WiFiS3.h>
#include "GUI.h"
#include "Buffer.h"
#include "State.h"
#include "Commands.h"
#include "CD4021.h"

// Declaration of Interrupt Service Routines and the encoder
void leftISR(); 

// Tasks (for scheduler to handle)
void telemetry(void *);
void sense(void *);

// Declare handles for each task, used to pass to task-analysing functions like uxTaskGetStackHighWaterMark()
TaskHandle_t telemetryHandle, senseHandle;

// Set up semaphores (I think we just need a mutex?)
SemaphoreHandle_t stateSemaphore;

void setup() {
  // Set up serial for debugging
  Serial.begin(115200);

  // Set up network
  wifi::initialiseAccessPoint();

  // Set up semaphore
  stateSemaphore = xSemaphoreCreateBinary();
  configASSERT(stateSemaphore != nullptr);
  xSemaphoreGive(stateSemaphore); 

  // Set up tasks
  xTaskCreate(telemetry, "Read from GUI", 2048, nullptr, 1, &telemetryHandle);
  xTaskCreate(sense, "Sense and drive", 2048, nullptr, 2, &senseHandle);

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
  Buffer<200> buffer;
  WiFiServer server(wifi::PORT);
  server.begin();

  WiFiClient GUI;

  while(true){
    // Reconnect if disconnected
    keep(GUI, server);
    // Read a command if there is one
    read(GUI, buffer);
    // Handle a command if there is one

    if(xSemaphoreTake(stateSemaphore, pdMS_TO_TICKS(5)) == pdTRUE){
      handle(buffer);
      sendDistance(GUI, state::currentDistance);
      xSemaphoreGive(stateSemaphore);
    }

    // Block this task for however long
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

// - TASK 2 -
void sense(void *parameters){
  // Declare physical constants
  constexpr float CIRCUMFERENCE = 20.4, DIAMETER = 6.5, AXLE = 14.5;

  // Hardware objects
  HCSR04 ears(8, 3); // PINS NEED TO BE CHANGED
  ROB12629 leftEncoder(2);
  CD4021 shifter(4, 5, 13);
  L293D driver(6,7,11,12,9,10);

  // Set up hardware
  driver.begin();
  ears.begin();
  shifter.begin();
  // We pass ISRs to the encoders
  leftEncoder.begin(leftISR);

  constexpr TickType_t period= pdMS_TO_TICKS(15);
  TickType_t lastWakeTime = xTaskGetTickCount();
  while(true){



    xTaskDelayUntil(&lastWakeTime, period); 
  }
}

// - OTHER FUNCTIONS -
