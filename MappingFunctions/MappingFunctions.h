#pragma once

#include <Arduino.h>
#include "L293D.h"
#include "CD4021.h"
#include "ROB12629.h"
#include "State.h"
#include "EEPROM.h"


bool _clocked  = false;
void _clockingISR(){
  _clocked = true;
}

namespace mapping{
  void forward(L293D& driver, CD4021& shifter, ROB12629& encoder, void (*resetShifter)()){
    // Serial.println("Inside forward");
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
      // Serial.println("Inside while loop");
      shifter.update(500000); encoder.update(500000);
      if(encoder.distance() < state.targetDistance){
        driver.leftForward(state.leftSpeedPercentage);
      } else {
        // Serial.println("Finished on encoder-side wheel");
        driver.leftBrake(L293D_BRAKE_TIME);
        encoderSideDone = true;
      } 

      if(shifter.distance() < state.targetDistance){
        driver.rightForward(state.rightSpeedPercentage);
      } else {
        driver.rightBrake(L293D_BRAKE_TIME);
        shifterSideDone = true;
      }
    }

    state.totalDistance += state.targetDistance;
  }

  void turnLeft(L293D& driver, CD4021& shifter, ROB12629& encoder, void (*resetShifter)()){
    static constexpr float SWEEP_CIRCUMFERENCE = 13.6 * PI;

    float targetSweep = SWEEP_CIRCUMFERENCE * state.targetAngle/360;

    encoder.reset();
    if(resetShifter){
      resetShifter();
    }

    bool encoderSideDone = false, shifterSideDone = false;
    while (!encoderSideDone || !shifterSideDone){
      // Serial.println("Inside while loop");
      shifter.update(500000); encoder.update(500000);
      if(encoder.distance() < targetSweep){
        driver.leftBackward(state.leftSpeedPercentage);
      } else {
        // Serial.println("Finished on encoder-side wheel");
        driver.leftBrake(50);
        encoderSideDone = true;
      } 

      if(shifter.distance() < targetSweep){
        driver.rightForward(state.rightSpeedPercentage);
      } else {
        driver.rightBrake(50);
        shifterSideDone = true;
      }
    }
  }

  void calibrate(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)()){

    // Fetch the speeds from last time (we're keeping them in EEPROM)
    if(EEPROM.begin(state.EEPROM_SIZE)){
      float left, right;
      EEPROM.get(state.LEFT_EEPROM_ADDRESS, left);
      EEPROM.get(state.RIGHT_EEPROM_ADDRESS, right);

      if(left < 0.0 || left > 1.0){
        state.leftSpeedPercentage = 0.25;
      } else {
        state.leftSpeedPercentage = left;
      }

      if(right < 0.0 || right > 1.0){
        state.rightSpeedPercentage = right; 
      }
    } else {
      state.leftSpeedPercentage = 0.25;
      state.rightSpeedPercentage = 0.3;
    }
  
    setWheels(driver, shifter, encoder, resetShifter, myISR);

  }

  void setWheels(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)()){
    // This function aligns the wheels to both be right on top of a count
    
    // Reset circuits
    encoder.reset();
    if(resetShifter) resetShifter();

    // - SET THE ENCODER-SIDE WHEEL -
    // Assign and interrupt to get the moment there's a pulse
    attachInterrupt(digitalPinToInterrupt(encoder.pin()), _clockingISR, CHANGE);

    while(!_clocked){
      driver.leftForward(0.3);
    }
    driver.leftBrake(L293D_BRAKE_TIME);
    _clocked = false;

    // Give the interrupt back to what it was before
    attachInterrupt(digitalPinToInterrupt(encoder.pin()), myISR, CHANGE);

    // - SET THE ANALOG-SIDE WHEEL - 
    uint8_t initialCount = shifter.shiftIn();
    while(initialCount == shifter.shiftIn()){
      driver.rightForward(0.3);
    }
    drive.rightBrake(L293D_BRAKE_TIME);

    // Reset circuits again just to make the function a "this gives us a fresh start" function
    encoder.reset();
    if(resetShifter) resetShifter();
  }

  
}




