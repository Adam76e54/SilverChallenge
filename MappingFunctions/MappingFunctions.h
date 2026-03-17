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
  void forward(L293D& driver);
  void left(L293D &driver);
  void right(L293D& driver);
  void calibrateRight(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)());
  void calibrateLeft(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)());
  void setWheels(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)());



  void forward(L293D &driver) {

    // NOTE: should probably allow for difference in wheels here too so that it ends up being roughly square

    unsigned long leftDuration_us = (unsigned long)(state.targetDistance / state.leftForwardCmPerSecond * 1e6f);
    unsigned long rightDuration_us = (unsigned long)(state.targetDistance / state.rightForwardCmPerSecond * 1e6f);

    auto start = micros();

    bool leftDone = false, rightDone = false;
    driver.forward(state.leftForwardPercentage, state.rightForwardPercentage);
    while (!leftDone || !rightDone) {
      auto time = micros() - start;
      if((time < leftDuration_us) && (time < rightDuration_us)){
        // keep going
      } else {
        leftDone = rightDone = true;
        driver.brake(L293D_BRAKE_TIME);
      }
    }

    state.totalDistance += state.targetDistance;
  }
  void backward(L293D &driver) {

    // NOTE: should probably allow for difference in wheels here too so that it ends up being roughly square

    unsigned long leftDuration_us = (unsigned long)(state.targetDistance / state.leftBackwardCmPerSecond * 1e6f);
    unsigned long rightDuration_us = (unsigned long)(state.targetDistance / state.rightBackwardCmPerSecond * 1e6f);

    auto start = micros();

    bool leftDone = false, rightDone = false;
    driver.backward(state.leftBackwardPercentage, state.rightBackwardPercentage);
    while (!leftDone || !rightDone) {
      auto time = micros() - start;
      if((time < leftDuration_us) && (time < rightDuration_us)){
        // keep going
      } else {
        leftDone = rightDone = true;
        driver.brake(L293D_BRAKE_TIME);
      }
    }

    state.totalDistance += state.targetDistance;
  }

  void left(L293D& driver){
    static constexpr float SWEEP_CIRCUMFERENCE = 13.6 * PI;

    float targetSweep = SWEEP_CIRCUMFERENCE * state.targetAngle/360;
    
    // NOTE need to account for difference in wheels here
    unsigned long rightDuration_us = (unsigned long)(targetSweep / state.rightForwardCmPerSecond * 1e6f);
    unsigned long leftDuration_us = (unsigned long)(targetSweep / state.leftBackwardCmPerSecond * 1e6f);

    unsigned long start = micros();
    bool leftDone = false, rightDone = false;
    while (!leftDone || !rightDone) {
      auto time = micros() - start;
      if(time < leftDuration_us){
        driver.leftBackward(state.leftBackwardPercentage);
      } else {
        leftDone = true;
        driver.leftBrake(10);
      }

      if(time < rightDuration_us){
        driver.rightForward(state.rightForwardPercentage);
      } else {
        rightDone = true;
        driver.rightBrake(10);
      }
    }

  }

  void right(L293D& driver){
    static constexpr float SWEEP_CIRCUMFERENCE = 13.6 * PI;

    float targetSweep = SWEEP_CIRCUMFERENCE * state.targetAngle/360;
    
    // NOTE need to account for difference in wheels here
    unsigned long rightDuration_us = (unsigned long)(targetSweep / state.rightBackwardCmPerSecond * 1e6f);
    unsigned long leftDuration_us = (unsigned long)(targetSweep / state.leftForwardCmPerSecond * 1e6f);

    unsigned long start = micros();
    bool leftDone = false, rightDone = false;
    while (!leftDone || !rightDone) {
      auto time = micros() - start;
      if(time < leftDuration_us){
        driver.leftForward(state.leftForwardPercentage);
      } else {
        leftDone = true;
        driver.leftCoast();
      }

      if(time < rightDuration_us){
        driver.rightBackward(state.rightBackwardPercentage);
      } else {
        rightDone = true;
        driver.rightCoast();
      }
    }

    driver.brake(L293D_BRAKE_TIME);
  }

  void fetchEEPROM(){
    // - LEFT FORWARD PERCENTAGE -
    float speed;
    EEPROM.get(state.LEFT_FORWARD_PERCENTAGE_ADDRESS, speed);

    if(isnan(speed)){
      speed = 0.3;
    }

    if(speed < 0.0 || speed > 1.0){
      state.leftForwardPercentage = 0.3;
    } else {
      state.leftForwardPercentage = speed;
    }

    // - RIGHT FORWARD PERCENTAGE -
    EEPROM.get(state.RIGHT_FORWARD_PERCENTAGE_ADDRESS, speed);

    if(isnan(speed)){
      speed = 0.3;
    }

    if(speed < 0.0 || speed > 1.0){
      state.rightForwardPercentage = 0.3;
    } else {
      state.rightForwardPercentage = speed;
    }

    // - LEFT BACKWARD PERCENTAGE -
    EEPROM.get(state.LEFT_BACKWARD_PERCENTAGE_ADDRESS, speed);

    if(isnan(speed)){
      speed = 0.3;
    }

    if(speed < 0.0 || speed > 1.0){
      state.leftBackwardPercentage = 0.3;
    } else {
      state.leftBackwardPercentage = speed;
    }

    // - RIGHT BACKWARD PERCENTAGE -
    EEPROM.get(state.RIGHT_BACKWARD_PERCENTAGE_ADDRESS, speed);

    if(isnan(speed)){
      speed = 0.3;
    }

    if(speed < 0.0 || speed > 1.0){
      state.rightBackwardPercentage = 0.3;
    } else {
      state.rightBackwardPercentage = speed;
    }

    EEPROM.get(state.LEFT_FORWARD_CM_PER_SECOND_ADDRESS, speed);
    state.leftForwardCmPerSecond = speed;

    EEPROM.get(state.RIGHT_FORWARD_CM_PER_SECOND_ADDRESS, speed);
    state.rightForwardCmPerSecond = speed;

    EEPROM.get(state.LEFT_BACKWARD_CM_PER_SECOND_ADDRESS, speed);
    state.leftBackwardCmPerSecond = speed;

    EEPROM.get(state.RIGHT_BACKWARD_CM_PER_SECOND_ADDRESS, speed);
    state.rightBackwardCmPerSecond = speed;
    
  }

void calibrate(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)()){
  float leftForwardPercentage = state.leftForwardPercentage;
  float rightForwardPercentage = state.rightForwardPercentage;

  float leftBackwardPercentage = state.leftBackwardPercentage;
  float rightbackwardPercentage = state.rightBackwardPercentage;

  constexpr uint8_t ROTATIONS_TO_USE = 5;
  constexpr float CIRCUMFERENCE = 20.4;

  constexpr unsigned long TARGET_TIME = 1e6 * ROTATIONS_TO_USE;

  constexpr float TOLERANCE = 0.01;

  bool forwardCalibrated = false, backwardCalibrated = false;


  float leftfHigh = 1.0f, leftfLow = 0.0f;
  float rightfHigh = 1.0f, rightfLow = 0.0f;

  float leftbHigh = 1.0f, leftbLow = 0.0f;
  float rightbHigh = 1.0f, rightbLow = 0.0f;

  // int8_t iteration = 0;
  // constexpr uint8_t MAX_ITERATIONS = 20;

  while(!forwardCalibrated || !backwardCalibrated){
    // - SET WHEELS ABOVE A COUNT -
    mapping::setWheels(driver, shifter, encoder, resetShifter, myISR);

    // - PERFORM FOWARD RUN -
    delay(2000);

    if(!forwardCalibrated){
      // - MEAURE FORWARD RUN -
      unsigned long leftTime = 0, rightTime = 0;
      measureForward(driver, shifter, encoder, leftForwardPercentage
        , rightForwardPercentage, leftTime, rightTime, ROTATIONS_TO_USE);

      // - ADJUST FORWARD SPEEDS -
      float leftError = (float)((long)leftTime - (long)TARGET_TIME) / (TARGET_TIME);  
      float rightError = (float)(rightTime - TARGET_TIME) / (TARGET_TIME);


      leftForwardPercentage = binaryAdjust(leftForwardPercentage, leftfHigh, leftfLow, leftError, TOLERANCE);

      rightForwardPercentage = binaryAdjust(rightForwardPercentage, rightfHigh, rightfLow, rightError, TOLERANCE);

      if(fabs(leftError) <= TOLERANCE && fabs(rightError) <= TOLERANCE){
        forwardCalibrated = true;

        state.leftForwardPercentage = leftForwardPercentage;
        state.rightForwardPercentage = rightForwardPercentage;

        state.leftForwardCmPerSecond = (CIRCUMFERENCE * ROTATIONS_TO_USE) / (leftTime / 1e6);
        state.rightForwardCmPerSecond = (CIRCUMFERENCE * ROTATIONS_TO_USE) / (rightTime / 1e6);
      }

    } else {
      // - IF CALIBRATED FORWARD, JUST MOVE FORWARD TO CLEAR WAY FOR BACKWARDS CALIBRATION
      state.targetDistance = CIRCUMFERENCE * ROTATIONS_TO_USE;
      mapping::forward(driver);
    }

    // - PERFORM BACKWARDS RUN -
    // - SET WHEELS ABOVE A COUNT -
    mapping::setWheels(driver, shifter, encoder, resetShifter, myISR);

    delay(2000);
    
    if(!backwardCalibrated){
      unsigned long leftTime = 0, rightTime = 0;
      measureBackward(driver, shifter, encoder, leftBackwardPercentage
        , rightbackwardPercentage, leftTime, rightTime, ROTATIONS_TO_USE);

      // - ADJUST BACKWARD SPEEDS -
      float leftError = (float)((long)leftTime - (long)TARGET_TIME) / (TARGET_TIME);  
      float rightError = (float)(rightTime - TARGET_TIME) / (TARGET_TIME);


      leftBackwardPercentage = binaryAdjust(leftBackwardPercentage, leftbHigh, leftbLow, leftError, TOLERANCE);
      rightbackwardPercentage = binaryAdjust(rightbackwardPercentage, rightbHigh, rightbLow, rightError, TOLERANCE);

      if(fabs(leftError) <= TOLERANCE && fabs(rightError) <= TOLERANCE){
        backwardCalibrated = true;

        state.leftBackwardPercentage = leftBackwardPercentage;
        state.rightBackwardPercentage = rightbackwardPercentage;

        state.leftBackwardCmPerSecond = (CIRCUMFERENCE * ROTATIONS_TO_USE) / (leftTime / 1e6);
        state.rightBackwardCmPerSecond = (CIRCUMFERENCE * ROTATIONS_TO_USE) / (rightTime / 1e6);
      }
    } else {
      // - IF CALIBRATED FORWARD, JUST MOVE FORWARD TO CLEAR WAY FOR BACKWARDS CALIBRATION
      state.targetDistance = CIRCUMFERENCE * ROTATIONS_TO_USE;
      mapping::backward(driver);
    }
  }

  // - UPDATE EEPROM -
  EEPROM.put(state.LEFT_FORWARD_PERCENTAGE_ADDRESS, state.leftForwardPercentage);
  EEPROM.put(state.LEFT_FORWARD_CM_PER_SECOND_ADDRESS, state.leftForwardCmPerSecond);

  EEPROM.put(state.LEFT_BACKWARD_PERCENTAGE_ADDRESS, state.leftBackwardPercentage);
  EEPROM.put(state.LEFT_BACKWARD_CM_PER_SECOND_ADDRESS, state.leftBackwardCmPerSecond);

  EEPROM.put(state.RIGHT_FORWARD_PERCENTAGE_ADDRESS, state.rightForwardPercentage);
  EEPROM.put(state.RIGHT_FORWARD_CM_PER_SECOND_ADDRESS, state.rightForwardCmPerSecond);

  EEPROM.put(state.RIGHT_BACKWARD_PERCENTAGE_ADDRESS, state.rightBackwardPercentage);
  EEPROM.put(state.RIGHT_BACKWARD_CM_PER_SECOND_ADDRESS, state.rightBackwardCmPerSecond);


}

void measureForward(L293D& driver, CD4021& shifter, ROB12629& encoder
  , float left, float right, unsigned long& leftTime, unsigned long& rightTime, const uint8_t ROTATIONS_TO_USE){
  
  unsigned long start = micros();

  bool finishedLeft = false, finishedRight = false;
  while(!finishedLeft || !finishedRight){
    auto shifted = shifter.shiftIn();
    auto encoded = encoder.count();

    if((encoded < encoder.COUNTS_PER_REV_ * ROTATIONS_TO_USE) && !finishedLeft){
      driver.leftForward(left);
    } else {
      driver.leftCoast();
      finishedLeft = true;

      leftTime = micros() - start;
    }

    if((shifted < shifter.COUNTS_PER_REV_ * ROTATIONS_TO_USE) && !finishedRight){
      driver.rightForward(right);
    } else {
      driver.rightCoast();
      finishedRight = true;

      rightTime = micros() - start;
    }
  }
}

void measureBackward(L293D& driver, CD4021& shifter, ROB12629& encoder
  , float left, float right, unsigned long& leftTime, unsigned long& rightTime, const uint8_t ROTATIONS_TO_USE ){
  
  unsigned long start = micros();

  bool finishedLeft = false, finishedRight = false;
  while(!finishedLeft || !finishedRight){
    auto shifted = shifter.shiftIn();
    auto encoded = encoder.count();

    if((encoded < encoder.COUNTS_PER_REV_ * ROTATIONS_TO_USE) && !finishedLeft){
      driver.leftBackward(left);
    } else {
      driver.leftCoast();
      finishedLeft = true;

      leftTime = micros() - start;
    }

    if((shifted < shifter.COUNTS_PER_REV_ * ROTATIONS_TO_USE) && !finishedRight){
      driver.rightBackward(right);
    } else {
      driver.rightCoast();
      finishedRight = true;

      rightTime = micros() - start;
    }
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

      // Serial.println("Setting left wheel");
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
      // Serial.print("Setting right wheel = ");
      // Serial.println(initialCount);
      driver.rightForward(0.3);
    }
    driver.rightBrake(L293D_BRAKE_TIME);

    // Reset circuits again just to make the function a "this gives us a fresh start" function
    encoder.reset();
    if(resetShifter) resetShifter();
  } 

}

float binaryAdjust(float current, float& high, float& low, float error, const float TOLERANCE){
  // Error here is (time - target)/target

  if(fabs(error) > TOLERANCE){
    if(error > 0){
      low = current;
    } else {
      high = current;
    }

    current = (low + high) / 2.0f;
    current = constrain(current, 0.0f, 1.0f);
  }
  return current; 
}



