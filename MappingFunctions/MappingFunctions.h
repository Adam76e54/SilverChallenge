#pragma once

#include <Arduino.h>
#include "L293D.h"
#include "CD4021.h"
#include "ROB12629.h"
#include "State.h"

constexpr float CIRCUMFERENCE = 20.4;
constexpr uint8_t COUNTS_PER_REVOLUTION = 4;

void forward(L293D driver, CD4021 shifter, ROB12629 encoder){

  // NOTE: it actually might be better to have faith in the speed rather than the count. 
  // If we know that she's going 20 cm/s then we can use timers which are much more accurate than encoders
  // So I might change this later. 

  // Get number of counts required
  uint8_t targetCounts = (state.targetDistance / CIRCUMFERENCE) * COUNTS_PER_REVOLUTION;

  encoder.reset();
  auto initialShifterCount = shifter.count();

  bool encoderSideDone = false, shifterSideDone = false;
  while (!encoderSideDone || !shifterSideDone){
    if(encoder.count() < targetCounts){
      driver.leftForward(state.leftSpeedPercentage);
    } else {
      driver.leftCoast();
      encoderSideDone = true;
    }

    if(shifter.count() - initialShifterCount < targetCounts){
      driver.rightForward(state.rightSpeedPercentage);
    } else {
      driver.rightCoast();
      shifterSideDone = true;
    }
  }
}

void turnLeft(L293D driver, CD4021 shifter, ROB12629 encoder){

}