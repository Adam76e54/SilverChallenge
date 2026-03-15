#include "Buggy.h"

L293D driver(6, 7, 11, 12, 9, 10);

HCSR04 ears(8, 3);

CD4021 shifter(A3, A2, A1);
// CD4021 shifter(4, 5, 13);

ROB12629 encoder(2);

int counter = 0;
void myISR(){
  encoder.onInterrupt();
}

void reset(); 

constexpr uint8_t RESET_PIN = 5;

void setup() {
  Serial.begin(115200);

  driver.begin();
  ears.begin();
  shifter.begin();
  encoder.begin(myISR);

  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);

  state.leftSpeedPercentage = 0.3; state.rightSpeedPercentage = 0.3;
  state.targetDistance = 30;
  state.targetAngle = 90;
}

void loop() {
  // reset();
  static unsigned long lastTime = 0;
  auto now = millis();
  
  if(now - lastTime > 10000) {
    mapping::forward(driver, shifter, encoder, reset);
    mapping::turnLeft(driver, shifter, encoder, reset);
    lastTime = now;
  }
}

void reset(){
  digitalWrite(RESET_PIN, HIGH);
  Serial.println("Set reset pin high");
  delayMicroseconds(1000);
  digitalWrite(RESET_PIN, LOW);
}
