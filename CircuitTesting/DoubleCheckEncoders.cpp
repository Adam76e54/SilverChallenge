#include "Buggy.h"

L293D driver(6, 7, 11, 12, 9, 10);

HCSR04 ears(8, 3);

CD4021 shifter(A3, A2, A1);
// CD4021 shifter(4, 5, 13);

// ROB12629 encoder(2);

int counter = 0;
void myISR(){
  counter++;
  // encoder.onInterrupt();
}
constexpr uint8_t RESET_PIN = A0;

void setup() {
  Serial.begin(115200);

  driver.begin();
  ears.begin();
  shifter.begin();
  // encoder.begin(myISR);

  pinMode(RESET_PIN, OUTPUT);
  pinMode(2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(2), myISR, RISING);
}

void loop() {
  // reset();
  // driver.drive(0.25,0.3);
  unsigned int interval = 5000000;
  shifter.update(interval);

  noInterrupts();
  auto count = counter;
  interrupts();

  Serial.print("Raw encoder count = ");
  Serial.print(count);
  Serial.print("  CD4021 count = ");
  Serial.println(shifter.count());
  // encoder.update(interval);
}

void reset(){
  digitalWrite(RESET_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(RESET_PIN, LOW);
}
