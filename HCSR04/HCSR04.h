#pragma once
#include <Arduino.h>

class HCSR04{
private:
  uint8_t trig_, echo_; 

  unsigned long lastPoll_ = 0;
  float lastDistance_ = 0;
public:
    HCSR04(uint8_t trig, uint8_t echo) 
    : trig_(trig), echo_(echo), lastPoll_(0), lastDistance_(0){
 
    }

  void begin(){
    pinMode(echo_, INPUT);
    pinMode(trig_, OUTPUT);
  }

  float centimeters() const{
    digitalWrite(trig_, LOW);
    delayMicroseconds(2);

    digitalWrite(trig_, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_, LOW);

    unsigned long duration = pulseIn(echo_, HIGH, 3000);
    
    if(duration == 0) return 999.0;//zero shouldn't happen so assume it's far away
    return (duration*0.0343)/2;
  }

  float millimeters() const{
    return centimeters()*10;
  }

  float meters() const{
    return centimeters()/100;
  }

  //interval is in milliseconds, defines how often poll can change lastDistance_
  float poll(const unsigned int interval){
    unsigned long now = millis();

    if(now - lastPoll_ >= interval){
      lastPoll_ = now;
      lastDistance_ = this->centimeters();
    }

    return lastDistance_;
  }
};