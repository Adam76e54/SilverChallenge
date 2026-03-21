#pragma once

#include <Arduino.h>

class Controller {
  private:
    float lastError;
    unsigned long lastTime; 
    float integral; 

    bool first;

    const float kp, ki, kd;

  public:

  Controller(float kp, float ki, float kd) 
    : lastError(0), lastTime(0), integral(0), first(true), kp(kp), ki(ki), kd(kd){
      // Empty
    }

  float PID(float error){
    auto now = millis();
    auto dt = (now - lastTime) * 1e-3; // units in seconds

    
    if(!first){
      integral += error * dt; 
    } else {
      first = false;
      integral = 0;
    }

    float derivative;
    if(dt > 0 && !first){
      derivative = (error - lastError) / dt;
    } else {
      first = false;
      derivative = 0;
    }

    float adjustment = kp * error
      + kd * derivative
      + ki * integral;

    lastError = error;
    lastTime = now;

    return adjustment;
  }

  void reset(){
    lastError = 0;
    lastTime = 0;
    integral = 0;
    first = true;
  }

};