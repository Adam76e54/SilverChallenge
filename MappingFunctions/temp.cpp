#include <Arduino.h>
#include "L293D.h"
#include "CD4021.h"
#include "ROB12629.h"
#include "State.h"
#include "EEPROM.h"

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

float binaryAdjust(float current, float& high, float& low, float error, const float TOLERANCE){
  // Error here is (time - target)/target
  float absoluteError = fabs(error);

  if(absoluteError > TOLERANCE){
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
