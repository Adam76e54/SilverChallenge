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
  void forward(L293D& driver, CD4021& shifter, ROB12629& encoder, void (*resetShifter)());
  void calibrate(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)());
  void setWheels(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)());


  void forward(L293D& driver, CD4021& shifter, ROB12629& encoder, void (*resetShifter)()){
    Serial.println("Inside forward");
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
      Serial.println("Inside while loop");
      shifter.update(500); encoder.update(500);
      if(encoder.distance() < state.targetDistance){
        driver.leftForward(state.leftSpeedPercentage);
      } else {
        Serial.println("Finished on encoder-side wheel");
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

  /* 
  something like this might be better:

void forwardTimed(L293D& driver,
                  float distance_cm) {

  float vL = state.leftSpeedCmPerSec;
  float vR = state.rightSpeedCmPerSec;
  float v  = min(vL, vR);  // use the limiting wheel

  unsigned long duration_us = (unsigned long)(distance_cm / v * 1e6f);

  unsigned long start = micros();
  while ((unsigned long)(micros() - start) < duration_us) {
    driver.leftForward(state.leftSpeedPercentage);
    driver.rightForward(state.rightSpeedPercentage);
    // small non-blocking maintenance if needed
  }

  driver.leftBrake(L293D_BRAKE_TIME);
  driver.rightBrake(L293D_BRAKE_TIME);
}
  */

  void turnLeft(L293D& driver, CD4021& shifter, ROB12629& encoder, void (*resetShifter)()){
    static constexpr float SWEEP_CIRCUMFERENCE = 13.6 * PI;

    float targetSweep = SWEEP_CIRCUMFERENCE * state.targetAngle/360;

    encoder.reset();
    if(resetShifter){
      resetShifter();
    }

    bool encoderSideDone = false, shifterSideDone = false;
    while (!encoderSideDone || !shifterSideDone){
      Serial.print("[Turning] "); 
      // Serial.print(" Target sweep = "); Serial.print(targetSweep);
      // Serial.print(" Left = "); Serial.print(state.leftSpeedPercentage);
      // Serial.print(" Right = "); Serial.println(state.rightSpeedPercentage);

      shifter.update(500); encoder.update(500);
      if(encoder.distance() < targetSweep){
        driver.leftBackward(state.leftSpeedPercentage);
      } else {
        Serial.println("Finished on encoder-side wheel");
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

    encoder.reset();
    if(resetShifter){
      resetShifter();
    }
  }

  void calibrateLeft(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)()){
    // - FETCH EEPROM - 
    float speed;
    EEPROM.get(state.LEFT_EEPROM_ADDRESS, speed);

    if(isnan(speed)){
      speed = 0.26;
    }

    if(speed < 0.0 || speed > 1.0){
      state.leftSpeedPercentage = 0.26;
    } else {
      state.leftSpeedPercentage = speed;
    }

    Serial.print("[Left calibrator] Speed = "); Serial.println(state.leftSpeedPercentage, 6);

    constexpr uint8_t NUMBER_OF_ROTATIONS_TO_USE = 5;
    constexpr float CIRCUMFERENCE = 20.4; 

    constexpr unsigned long TOLERANCE = 100e3, TARGET_TIME = 1e6 * NUMBER_OF_ROTATIONS_TO_USE;
    float proportionalCorrector = 0.15f;
    unsigned long start = 0, end = 0;

    // - RUN CALIBRATION
    bool calibrated = false;
    while(!calibrated){
      bool finishedRun = false;

      // - SET WHEELS TO BE RIGHT ABOVE A COUNT -
      mapping::setWheels(driver, shifter, encoder, resetShifter, myISR);
      encoder.reset();

      // - PERFORM A RUN - 
      start = micros();
      while(!finishedRun){
        if(encoder.count() < encoder.COUNTS_PER_REV_ * NUMBER_OF_ROTATIONS_TO_USE){
          driver.forward(state.leftSpeedPercentage, state.rightSpeedPercentage);
        } else {
          driver.brake(L293D_BRAKE_TIME);
          end = micros();
          finishedRun = true;
        }
        // - END RUN -
      } 

      Serial.print(" Finished a run "); 
      Serial.println();

      // - COMPUTE TIME TAKEN -
      unsigned long time = (end >= start) ? (end - start) 
          : (UINT32_MAX - start + 1u + end);
      
      Serial.print(" Time taken = "); Serial.print(time);
      Serial.println();

      long error = (long)time - (long)TARGET_TIME;
      unsigned long absoluteError = labs(error);

      Serial.print(" Abs Err = "); Serial.print(absoluteError);
      Serial.println();


      // - MAKE ADJUSTMENT -
      if(absoluteError > TOLERANCE){
        
        
        float relativeError = (float)error / (float)TARGET_TIME;
        constrain(relativeError, -0.2, 0.2);

        if(fabs(relativeError) < 0.1){
          proportionalCorrector = 0.1;
        }

        Serial.print(" Rel Err = "); Serial.print(relativeError, 6);
        Serial.println();

        state.leftSpeedPercentage = state.leftSpeedPercentage + proportionalCorrector * relativeError;
        state.leftSpeedPercentage = constrain(state.leftSpeedPercentage, 0.0f, 1.0f);
        
        Serial.print(" New speed = "); Serial.print(state.leftSpeedPercentage ,6);
        Serial.println();
        
        // - TURN AROUND FOR NEXT RUN
        state.targetAngle = 180;    
        // turnLeft(driver, shifter, encoder, resetShifter);

        Serial.print(" turned around ");
        Serial.println();
      } else {
        calibrated = true;

        state.leftCmPerSecond = NUMBER_OF_ROTATIONS_TO_USE * CIRCUMFERENCE / (float)(time * 1e-6);
        
        Serial.print(" Calibrated! ");
        Serial.print(" New speed = "); Serial.print(state.leftCmPerSecond, 6);
        Serial.println();

        Serial.print(" Calibrated! ");
        Serial.print(" New speed = "); Serial.print(state.leftSpeedPercentage, 6);
        Serial.println();
      }

    }

    // - UPDATE EEPROM -
    if(speed != state.leftSpeedPercentage){
      EEPROM.put(state.LEFT_EEPROM_ADDRESS, state.leftSpeedPercentage);

      Serial.print(" Updated EEPROM");
      Serial.println();
    }

    float cmPerSecond;
    EEPROM.get(state.LEFT_CM_PER_SECOND_EPROM_ADDRESS, cmPerSecond);
    if(cmPerSecond != state.leftCmPerSecond){
      EEPROM.put(state.LEFT_CM_PER_SECOND_EPROM_ADDRESS, state.leftCmPerSecond);
    }
  }

  void calibrateRight(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)()){

    // - FETCH EEPROM - 
    float speed; // "speed" is a percentage of 255 here
    EEPROM.get(state.RIGHT_EEPROM_ADDRESS, speed);

    if(isnan(speed)){
      speed = 0.27;
    }

    if(speed < 0.0 || speed > 1.0){
      state.rightSpeedPercentage = 0.27;
    } else {
      state.rightSpeedPercentage = speed;
    }

    Serial.print("[right calibrator] Speed = "); Serial.println(state.rightSpeedPercentage, 6);


    constexpr uint8_t NUMBER_OF_ROTATIONS_TO_USE = 5;
    constexpr float CIRCUMFERENCE = 20.4;

    constexpr unsigned long TOLERANCE = 150e3, TARGET_TIME = 1e6 * NUMBER_OF_ROTATIONS_TO_USE;
    float proportionalCorrector = 0.15f;
    unsigned long start = 0, end = 0;

    // - RUN CALIBRATION
    bool calibrated = false;
    while(!calibrated){
      bool finishedRun = false;

      // - SET WHEELS TO BE RIGHT ABOVE A COUNT -
      mapping::setWheels(driver, shifter, encoder, resetShifter, myISR);
      resetShifter();

      // - PERFORM A RUN - 
      start = micros();
      while(!finishedRun){
        auto shift = shifter.shiftIn();
        if(shift < shifter.COUNTS_PER_REV_ * NUMBER_OF_ROTATIONS_TO_USE){
          driver.forward(state.leftSpeedPercentage, state.rightSpeedPercentage);
          Serial.println(shift);
        } else {
          driver.brake(L293D_BRAKE_TIME);
          end = micros();
          finishedRun = true;
        }
        // - END RUN -
      } 

      Serial.print(" Finished a run "); 
      Serial.println();

      // - COMPUTE TIME TAKEN -
      unsigned long time = (end >= start) ? (end - start) 
          : (UINT32_MAX - start + 1u + end);
      
      Serial.print(" Time taken = "); Serial.print(time);
      Serial.println();

      long error = (long)time - (long)TARGET_TIME;
      unsigned long absoluteError = labs(error);

      Serial.print(" Abs Err = "); Serial.print(absoluteError);
      Serial.println();


      // - MAKE ADJUSTMENT -
      if(absoluteError > TOLERANCE){
        
        float relativeError = (float)error / (float)TARGET_TIME;
        constrain(relativeError, -0.2, 0.2);

        if(fabs(relativeError) < 0.1){
          proportionalCorrector = 0.1;
        }

        Serial.print(" Rel Err = "); Serial.print(relativeError, 6);
        Serial.println();

        state.rightSpeedPercentage = state.rightSpeedPercentage + proportionalCorrector * relativeError;
        state.rightSpeedPercentage = constrain(state.rightSpeedPercentage, 0.0f, 1.0f);
        
        Serial.print(" New speed = "); Serial.print(state.rightSpeedPercentage ,6);
        Serial.println();
        
        // - TURN AROUND FOR NEXT RUN
        state.targetAngle = 180;    
        // turnLeft(driver, shifter, encoder, resetShifter);

        Serial.print(" turned around ");
        Serial.println();
      } else {
        calibrated = true;

        state.rightCmPerSecond = NUMBER_OF_ROTATIONS_TO_USE * CIRCUMFERENCE / (float)(time * 1e-6);

        Serial.print(" Calibrated! ");
        Serial.print(" New speed = "); Serial.print(state.rightCmPerSecond, 6);
        Serial.println();

        Serial.print(" Calibrated right side! ");
        Serial.print(" New speed = "); Serial.print(state.rightSpeedPercentage, 6);
        Serial.println();
      }



    }

    // - UPDATE EEPROM -
    if(speed != state.rightSpeedPercentage){
      EEPROM.put(state.RIGHT_EEPROM_ADDRESS, state.rightSpeedPercentage);

      Serial.print(" Updated EEPROM");
      Serial.println();
    }

    float cmPerSecond;
    EEPROM.get(state.RIGHT_CM_PER_SECOND_EPROM_ADDRESS, cmPerSecond);
    if(cmPerSecond != state.rightCmPerSecond){
      EEPROM.put(state.RIGHT_CM_PER_SECOND_EPROM_ADDRESS, state.rightCmPerSecond);
    }

    
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




