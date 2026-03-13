#include "Buggy.h"

L293D driver(6, 7, 11, 12, 9, 10);

HCSR04 ears(8, 3);

CD4021 shifter(4, 5, 13);

constexpr uint8_t RESET_PIN = A0;

void setup() {
  Serial.begin(115200);

  driver.begin();
  ears.begin();
  shifter.begin();

  pinMode(RESET_PIN, OUTPUT);

    
  // On Uno R4, pin 2 is still INT0, so this is fine
}

void loop() {
  // reset();
  driver.drive(0.4,0.4);
  unsigned int interval = 2000000;
  shifter.update(interval);
}

void reset(){
  digitalWrite(RESET_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(RESET_PIN, LOW);
}
