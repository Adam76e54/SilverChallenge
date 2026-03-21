#include "Buggy.h"

constexpr uint8_t N = 200;
Buffer<N> in;
WiFiServer server(wifi::PORT);
WiFiClient GUI;

L293D driver(6, 7, 11, 12, 9, 10);

HCSR04 ears(8, A5);

CD4021 shifter(3, 5, 4);

ROB12629 encoder(2);

void myISR(){
  encoder.onInterrupt();
}

constexpr uint8_t RESET_PIN = 13;

void setup () {
  Serial.begin(115200);

  wifi::initialiseAccessPoint();
  server.begin();

  driver.begin();
  ears.begin();

  shifter.begin();
  encoder.begin(myISR);

  mapping::fetchEEPROM();
  state.mode = MAPPING;

  Serial.println(state.leftForwardPercentage);
  Serial.println(state.rightForwardPercentage);

  mapping::setWheels(driver, shifter, encoder, reset, myISR);
}

void loop () {
  keep(GUI, server);

  read(GUI, in);

  handle(in);

  switch (state.activity){

    case IDLE:
      // do nothing
    break;

    case FORWARD:
      mapping::forward(driver, ears, encoder);

      char message[64];
      snprintf(message, sizeof(message), "Speed = %f", (state.leftForwardCmPerSecond + state.rightForwardCmPerSecond) / 2);
      sendEvent(GUI, message);

      state.activity = IDLE;
    break;

    case LEFT:
      state.activity = IDLE;
      mapping::left(driver);
    break;

    case RIGHT:
      state.activity = IDLE;
      mapping::right(driver);
    break;

    case MATCHING:
      state.activity = IDLE;
      mapping::matchSpeeds(driver, shifter, encoder, reset, myISR);
    break;
    
    case CALIBRATING_LEFT:
      state.activity = IDLE;
      mapping::calibrateLeft(driver, shifter, encoder, reset, myISR);
    break;

    case CALIBRATING_RIGHT:
      state.activity = IDLE;
      mapping::calibrateRight(driver, shifter, encoder, reset, myISR);
    break;
    
  }

  auto now = millis();
  static auto then = now;
  if(now - then >= 10000){
    char message[64];
    snprintf(message, sizeof(message), "Total distance = %f", state.totalDistance);
    
    sendEvent(GUI, message);

    snprintf(message, sizeof(message), "Speed = %f", 0.0);
    sendEvent(GUI, message);

    GUI.print(comm::LEFT_SPEED); GUI.print(comm::DELIMITER); GUI.println(state.leftForwardPercentage);
    GUI.print(comm::RIGHT_SPEED); GUI.print(comm::DELIMITER); GUI.println(state.rightForwardPercentage);

    then = now;
  }
}

void reset(){
  digitalWrite(RESET_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(RESET_PIN, LOW);
}
