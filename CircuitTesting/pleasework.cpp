#include "Buggy.h"

// L293D: (enableA, enableB, in1, in2, in3, in4) – adapt to your wiring
L293D driver(6, 7, 11, 12, 9, 10);

// HCSR04(trigger, echo)
HCSR04 ears(8, 4);

// ROB12629 encoder on external interrupt pin (Uno R4: D2 or D3)
ROB12629 encoder(2);

// CD4021(shData, shclockPin, shlatchPin)
// Use 8 instead of 5 for the clockPin to avoid conflict with HCSR04
CD4021 shifter(4, 5, 13);

uint8_t counter = 0;
void leftISR() {
  ++counter;
}

void setup() {
  Serial.begin(115200);

  driver.begin();
  ears.begin();
  shifter.begin();
  // On Uno R4, pin 2 is still INT0, so this is fine
  encoder.begin(leftISR);
}

void loop() {
  constexpr float CIRCUMFERENCE = 20.4;

  driver.drive(0.6,0.6);

  static uint8_t lastCount = 0; 
  static unsigned long lastTime = 0;

  noInterrupts();
  auto count = counter;
  interrupts();
  // auto count = shifter.shiftIn();


  auto now = millis();

  unsigned long dt = now - lastTime;
  if(now - lastTime >= 20){
    uint8_t dc = count - lastCount;
    
    Serial.print("dc = ");
    Serial.print(dc);
    Serial.print("; dt = ");
    Serial.print(dt);
    Serial.print("   count = ");
    Serial.print(count);
    Serial.print("; lastCount = ");
    Serial.println(lastCount);

    lastCount = count; 
    lastTime = now; 
  }
}

