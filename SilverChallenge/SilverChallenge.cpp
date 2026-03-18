#include "Buggy.h"

constexpr uint8_t N = 200;
Buffer<N> in;
WiFiServer server(wifi::PORT);
WiFiClient GUI;

L293D driver(6, 7, 11, 12, 9, 10);

HCSR04 ears(8, 3);

CD4021 shifter(3, 5, 4);

ROB12629 encoder(2);

constexpr uint8_t RESET_PIN = 13;

void setup () {
  Serial.begin(115200);

  wifi::initialiseAccessPoint();
  server.begin();

  driver.begin();
  ears.begin();

  

  mapping::fetchEEPROM();
  state.mode = MAPPING;
}

void loop () {
  keep(GUI, server);

  read(GUI, in);

  handle(in);

  constexpr uint8_t US_POLLING_RATE = 150;

  switch (state.activity){
    case IDLE:
      // do nothing
    break;

    case FORWARD:
      if(safe(ears)){
        mapping::forward(driver);
        state.activity = IDLE;
      }
    break;

    case LEFT:
      mapping::left(driver);
    break;

    case RIGHT:
      mapping::right(driver);
    break;
  }
}

void reset(){
  digitalWrite(RESET_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(RESET_PIN, LOW);
}

bool safe(HCSR04 &ears){

  return false;
}