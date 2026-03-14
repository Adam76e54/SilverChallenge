#pragma once

#include <Arduino.h>
#include "L293D.h"
#include "CD4021.h"
#include "ROB12629.h"
#include "State.h"

constexpr float CIRCUMFERENCE = 20.4;
constexpr uint8_t COUNTS_PER_REVOLUTION = 4;

namespace map{
  void forward(L293D driver, CD4021 shifter, ROB12629 encoder, void (*resetShifter)()){

    // NOTE: it actually might be better to have faith in the speed rather than the count. 
    // If we know that she's going 20 cm/s then we can use timers which are much more accurate than encoders
    // So I might change this later. 

    // Get number of counts required
    encoder.reset();
    if(resetShifter){
      resetShifter();
    }

    bool encoderSideDone = false, shifterSideDone = false;
    while (!encoderSideDone || !shifterSideDone){
      shifter.update(5000); encoder.update(5000);
      if(encoder.distance() < state.targetDistance){
        driver.leftForward(state.leftSpeedPercentage);
      } else {
        driver.leftBrake(50);
        encoderSideDone = true;
      } 

      if(shifter.distance() < state.targetDistance){
        driver.rightForward(state.rightSpeedPercentage);
      } else {
        driver.rightBrake(50);
        shifterSideDone = true;
      }
    }
  }

  void turnLeft(L293D driver, CD4021 shifter, ROB12629 encoder){

  }
}