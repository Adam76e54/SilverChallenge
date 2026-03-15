#pragma once

#include <Arduino.h>
#include "L293D.h"
#include "CD4021.h"
#include "ROB12629.h"
#include "State.h"
#include "EEPROM.h"


volatile bool _clocked  = false;
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
    float left, right;
    EEPROM.get(state.LEFT_EEPROM_ADDRESS, left);
    EEPROM.get(state.RIGHT_EEPROM_ADDRESS, right);

    if(left < 0.0 || left > 1.0){
      state.leftSpeedPercentage = 0.25;
    } else {
      state.leftSpeedPercentage = left;
    }

    if(right < 0.0 || right > 1.0){
      state.rightSpeedPercentage = 0.3; 
    } else {
      state.rightSpeedPercentage = right;
    }


    // Set the wheels for calibration
    setWheels(driver, shifter, encoder, resetShifter, myISR);
    delayMicroseconds(500);
    encoder.reset();
    resetShifter();


    // Tolerance = 10 ms, target time = 1e3 ms
    constexpr long TOLERANCE = 11, TARGET_TIME = 1000;
    unsigned long shifterStart = 0, shifterEnd = 0, encoderStart = 0, encoderEnd = 0;
    
    bool shifterCalibrated = false;
    bool encoderCalibrated = false;

    while(!shifterCalibrated || !encoderCalibrated){
      // While either wheel isn't at 20.4 cm/s, keep calibrating

      bool shifterDone = false, encoderDone = false;

      shifterStart = encoderStart = millis();
      shifterEnd = 0, encoderEnd = 0;
      while(!shifterDone || !encoderDone){
        // Perform a revolution on both wheels, measure the time difference
        if(shifter.shiftIn() < shifter.COUNTS_PER_REV_){
          driver.rightForward(state.rightSpeedPercentage);
        } else {
          driver.rightBrake(L293D_BRAKE_TIME);
          shifterEnd = millis();
          shifterDone = true;
        }

        if(encoder.count() < encoder.COUNTS_PER_REV_){
          driver.leftForward(state.leftSpeedPercentage);
        } else {
          driver.leftBrake(L293D_BRAKE_TIME);
          encoderEnd = millis();
          encoderDone = true;
        }

        // End of "Perform the run" loop
      } 

      long shifterTime = (long)shifterEnd - (long)shifterStart;
      long encoderTime = (long)encoderEnd - (long)encoderStart;

      shifterCalibrated = labs(shifterTime - TARGET_TIME) < TOLERANCE;
      encoderCalibrated = labs(encoderTime - TARGET_TIME) < TOLERANCE;

      if(!shifterCalibrated || !encoderCalibrated){
        // If not calibrated after a run then compute error, turn around and reset wheels for next run
        // NOTE: since it's not calibrated, the turning may be awful so it needs to be watched when calibrating

        /*
        NOTE: because the PWM to actual speed relationship is non-linear (friction and shit)
        we'll use a corrector factor to tune how big of an effect the relative error should have
        */
        constexpr float PROPORTIONAL_CORRECTOR = 0.5f;
        if(!shifterCalibrated){
          float error = (shifterTime - TARGET_TIME) / TARGET_TIME;

          state.rightSpeedPercentage = state.rightSpeedPercentage + PROPORTIONAL_CORRECTOR * error;
          state.rightSpeedPercentage = constrain(state.rightSpeedPercentage, 0.0f, 1.0f);
        }

        if(!encoderCalibrated){
          float error = (encoderTime - TARGET_TIME) / TARGET_TIME;

          state.leftSpeedPercentage = state.leftSpeedPercentage + PROPORTIONAL_CORRECTOR * error;
          state.leftSpeedPercentage = constrain(state.leftSpeedPercentage, 0.0f, 1.0f);
        }


        state.targetAngle = 180;
        turnLeft(driver, shifter, encoder, resetShifter);

        setWheels(driver, shifter, encoder, resetShifter, myISR);

        // End of "correct the error" if statement
      }

      // End of big "do until calibrated" loop
    }  

    // Remember the calibrated speeds
    EEPROM.put(state.LEFT_EEPROM_ADDRESS, state.leftSpeedPercentage);
    EEPROM.put(state.RIGHT_EEPROM_ADDRESS, state.rightSpeedPercentage);
  }

  void setWheels(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)()){
    // This function aligns the wheels to both be right on top of a count
    
    // Reset circuits
    encoder.reset();
    if(resetShifter) resetShifter();

    // - SET THE ENCODER-SIDE WHEEL -
    // Assign and interrupt to get the moment there's a pulse
    attachInterrupt(digitalPinToInterrupt(encoder.pin()), _clockingISR, CHANGE);

    noInterrupts();
    bool localClocked = _clocked;
    interrupts();

    while(!localClocked){
      driver.leftForward(0.3);

      noInterrupts();
      localClocked = _clocked;
      interrupts();
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
    driver.rightBrake(L293D_BRAKE_TIME);

    // Reset circuits again just to make the function a "this gives us a fresh start" function
    encoder.reset();
    if(resetShifter) resetShifter();
  } 

}




