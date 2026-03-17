#include "Buggy.h"

L293D driver(6, 7, 11, 12, 9, 10);

HCSR04 ears(8, 3);

CD4021 shifter(A3, A2, A1);

ROB12629 encoder(2);

void myISR(){
  encoder.onInterrupt();
}

constexpr uint8_t RESET_PIN = A0;
void reset();

void setup() {
  Serial.begin(115200);

  driver.begin();
  ears.begin();
  shifter.begin();
  encoder.begin(myISR);

  pinMode(RESET_PIN, OUTPUT);

  EEPROM.begin();
  mapping::calibrate(driver, shifter, encoder, reset, myISR);

  // mapping::setWheels(driver, shifter, encoder, reset, myISR);
}

void loop() {
  // reset();
  // driver.drive(state.leftSpeedPercentage, state.rightSpeedPercentage);


  Serial.print("Left percentage = ");
  Serial.print(state.leftSpeedPercentage);
  Serial.print("  Right percentage = ");
  Serial.println(state.rightSpeedPercentage);
}

void reset(){
  digitalWrite(RESET_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(RESET_PIN, LOW);
}
