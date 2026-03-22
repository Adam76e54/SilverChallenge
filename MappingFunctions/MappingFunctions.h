#pragma once

#include <Arduino.h>
#include "L293D.h"
#include "CD4021.h"
#include "ROB12629.h"
#include "State.h"
#include "EEPROM.h"
#include "Controller.h"
#include "GUI.h"


volatile bool _clocked  = false;
void _clockingISR(){
  _clocked = true;
}

namespace mapping{
  void forward(L293D &driver, HCSR04& ears, CD4021& shifter, ROB12629& encoder, void (*resetShifter)());
  void left(L293D &driver);
  void right(L293D& driver);
  void calibrateRight(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)());
  void calibrateLeft(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)());
  void matchSpeeds(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)());
  void setWheels(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)());
  float binaryAdjust(float current, float& high, float& low, float error, const float TOLERANCE);

  void measureBackward(L293D& driver, CD4021& shifter, ROB12629& encoder
    , float left, float right, unsigned long& leftTime, unsigned long& rightTime, const uint8_t ROTATIONS_TO_USE );
  void measureForward(L293D& driver, CD4021& shifter, ROB12629& encoder
    , float left, float right, unsigned long& leftTime, unsigned long& rightTime, const uint8_t ROTATIONS_TO_USE );
    
  bool safe(HCSR04 &ears);

  Controller controller(0.5f, 0.1f, 0.1f);


  void forward(L293D &driver, HCSR04& ears, CD4021& shifter, ROB12629& encoder, void (*resetShifter)()) {

    // NOTE: should probably allow for difference in wheels here too so that it ends up being roughly square
    encoder.reset();
    resetShifter();
    controller.reset();

    // I made a mess of the maths here because I kept tweaking small things to get it to work
    // Don't want to risk breaking it right now so won't clean it up
    constexpr auto CmPerCount = (20.4 / encoder.COUNTS_PER_REV_);

    float revTarget = state.targetDistance / CmPerCount / (float)encoder.COUNTS_PER_REV_;

    float encoderSide = 0;
    uint8_t lastCount = 0;
    while (encoderSide < (revTarget)) {

      auto count = encoder.count();
      encoderSide = (float)count / (float)encoder.COUNTS_PER_REV_;
      float shifterSide = (float)shifter.shiftIn() / (float)shifter.COUNTS_PER_REV_;

      float difference = encoderSide - shifterSide;
      
      if(mapping::safe(ears)){

        float adjustment = controller.PID(difference);

        float left = state.leftForwardPercentage - adjustment;
        float right = state.rightForwardPercentage + adjustment;

        driver.forward(left, right);

      } else {

        driver.brake(L293D_BRAKE_TIME);
      
      }

      auto now = millis();
      static unsigned long then = 0;
      if(now - then >= 1000){
        auto countChange = count - lastCount;
        
        float speed = (float)(countChange) / (float)((long)now - (long)then);
        Serial.print(comm::CURRENT_SPEED); Serial.print(':'); Serial.println(speed);
        GUI.print(comm::CURRENT_SPEED); GUI.print(':'); GUI.println(speed);
      }

    }
    
    driver.brake(L293D_BRAKE_TIME);
    
    state.totalDistance += CmPerCount / revTarget * encoder.COUNTS_PER_REV_;

    Serial.print(comm::TOTAL_DISTANCE); Serial.print(':'); Serial.println(state.totalDistance);
    GUI.print(comm::TOTAL_DISTANCE); GUI.print(':'); GUI.println(state.totalDistance);

  }

  void left(L293D& driver){
    auto start = micros();
    while(true){

      if(micros() - start <= state.leftTurnTime){
        driver.leftBackward(state.leftBackwardPercentage);
        driver.rightForward(state.rightForwardPercentage);
      } else {
        driver.brake(L293D_BRAKE_TIME);
        break;
      }

    }

  }

  void right(L293D& driver){
    auto start = micros();
    while(true){

      if(micros() - start <= state.righTurnTime){
        driver.leftForward(state.leftForwardPercentage);
        driver.rightBackward(state.rightBackwardPercentage);
      } else {
        driver.brake(L293D_BRAKE_TIME);
        break;
      }

    }

  }

  void calibrateRight(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)()){

    constexpr uint8_t ITERATIONS = 4;

    unsigned long times[ITERATIONS];

    for(uint8_t i = 0; i < ITERATIONS; ++i){
      bool left = false, right = false;
      Serial.println(i);

      delay(1500);

      mapping::setWheels(driver, shifter, encoder, resetShifter, myISR);

      delay(1500);
      
      auto start = micros();
      auto lEnd = start;

      while (!left || !right) {

        if(encoder.count() < encoder.COUNTS_PER_REV_ / 2 ){
          driver.leftForward(state.leftForwardPercentage);
          driver.rightBackward(state.rightBackwardPercentage);
        } else {
          driver.coast();
          lEnd = micros();
          left = true; right = true;
        }

      }

      times[i] = lEnd - start;

    }

    unsigned long tally = 0;
    for(uint8_t i = 0; i < ITERATIONS; ++i){
      tally += times[i];
    }

    state.righTurnTime = tally / ITERATIONS;
    EEPROM.put(state.RIGHT_TURN_TIME_ADDRESS, state.righTurnTime);
  }

  void calibrateLeft(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)()){

    constexpr uint8_t ITERATIONS = 4;

    unsigned long times[ITERATIONS];

    for(uint8_t i = 0; i < ITERATIONS; ++i){
      bool left = false, right = false;

      delay(1500);

      mapping::setWheels(driver, shifter, encoder, resetShifter, myISR);
      
      delay(1500);

      auto start = micros();
      auto lEnd = start;

      while (!left || !right) {
        Serial.println("Entered while loop");

        if(encoder.count() < encoder.COUNTS_PER_REV_ / 2 ){
          driver.leftBackward(state.leftBackwardPercentage);
          driver.rightForward(state.rightForwardPercentage);
        } else {
          driver.coast();
          lEnd = micros();
          left = true; right = true;
        }

        // if(shifter.shiftIn() < shifter.COUNTS_PER_REV_ / 2){
        // } else {
        //   driver.rightCoast();
        //   rEnd = micros();
        //   right = true;
        // }
      }

      times[i] = lEnd - start;

    }

    unsigned long tally = 0;
    for(uint8_t i = 0; i < ITERATIONS; ++i){
      tally += times[i];
    }

    state.leftTurnTime = tally / ITERATIONS;
    EEPROM.put(state.LEFT_TURN_TIME_ADDRESS, state.leftTurnTime);
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

    EEPROM.get(state.RIGHT_TURN_TIME_ADDRESS, state.righTurnTime);
    EEPROM.get(state.LEFT_TURN_TIME_ADDRESS, state.leftTurnTime);
  }

  void saveEEPROM(){
    EEPROM.put(state.LEFT_FORWARD_PERCENTAGE_ADDRESS, state.leftForwardPercentage);

    EEPROM.put(state.RIGHT_FORWARD_PERCENTAGE_ADDRESS, state.rightForwardPercentage);
  }
  
  void matchSpeeds(L293D& driver, CD4021 &shifter, ROB12629 &encoder, void (*resetShifter)(), void (*myISR)()){
    constexpr uint8_t ROTATIONS_TO_USE = 3;

    constexpr unsigned long TARGET_TIME = 1e6 * ROTATIONS_TO_USE;

    constexpr float TOLERANCE = 0.02;

    bool forwardCalibrated = false, backwardCalibrated = false;


    // These are brackest for binaryAdjust() 
    float leftfHigh = 0.6f, leftfLow = 0.2f;
    float rightfHigh = 0.6f, rightfLow = 0.2f;

    float leftbHigh = 0.6f, leftbLow = 0.2f;
    float rightbHigh = 0.6f, rightbLow = 0.2f;

    // Iteration capping so it can't loop forever if binaryAdjust() isn't finding the target speed
    int8_t iteration = 0;
    constexpr uint8_t MAX_ITERATIONS = 10;

    // if lastSpeed = adjustedSpeed, we've calibrated
    float lastFL = 0, lastFR = 0, lastBL = 0, lastBR = 0;

    // - CALIBRATION LOOP -
    while((!forwardCalibrated || !backwardCalibrated) && iteration < MAX_ITERATIONS){
      ++iteration;

      constexpr uint8_t RUNS = 3;
      // backwardLefTimes = blt, forwardRightTimes = frt, etc.
      unsigned long blt[RUNS], brt[RUNS], flt[RUNS], frt[RUNS];

      for(uint8_t i = 0; i < RUNS; ++i){
        mapping::setWheels(driver, shifter, encoder, resetShifter, myISR);

        mapping::measureForward(driver, shifter, encoder
          , state.leftForwardPercentage, state.rightForwardPercentage, flt[i], frt[i], ROTATIONS_TO_USE);

        mapping::setWheels(driver, shifter, encoder, resetShifter, myISR);

        mapping::measureBackward(driver, shifter, encoder
          , state.leftBackwardPercentage, state.rightBackwardPercentage, blt[i], brt[i], ROTATIONS_TO_USE);
      }

      unsigned long bltTally = 0, brtTally = 0, fltTally = 0, frtTally = 0;
      for(uint8_t i = 0; i < RUNS; ++i){ 
        bltTally += blt[i];
        brtTally += brt[i];
        fltTally += flt[i];
        frtTally += frt[i];
      }

      unsigned long br = brtTally / RUNS;
      unsigned long bl = bltTally / RUNS;
      unsigned long fr = frtTally / RUNS;
      unsigned long fl = fltTally / RUNS;

      auto makeError = [TARGET_TIME](unsigned long measurement) {
        return (float)((long)measurement - (long)TARGET_TIME) / TARGET_TIME;
      };

      state.leftForwardPercentage = binaryAdjust(state.leftForwardPercentage, leftfHigh, leftfLow
        , makeError(fl), TOLERANCE);

      state.rightForwardPercentage = binaryAdjust(state.rightForwardPercentage, rightfHigh, rightfLow
        , makeError(fr), TOLERANCE);

      if(lastFL == state.leftForwardPercentage && lastFR == state.rightForwardPercentage){
        forwardCalibrated = true;
      } else {
        lastFL = state.leftForwardPercentage;
        lastFR = state.rightForwardPercentage;
      }

      state.rightBackwardPercentage = binaryAdjust(state.rightBackwardPercentage, rightbHigh, rightbLow
        , makeError(br), TOLERANCE);

      state.leftBackwardPercentage = binaryAdjust(state.leftBackwardPercentage, leftbHigh, leftbLow
        , makeError(bl), TOLERANCE);

      if(lastBL == state.leftBackwardPercentage && lastBR == state.rightBackwardPercentage){
        backwardCalibrated = true;
      } else {
        lastBL = state.leftBackwardPercentage;
        lastBR = state.rightBackwardPercentage;
      }

    }

    // - UPDATE EEPROM -
    EEPROM.put(state.LEFT_FORWARD_PERCENTAGE_ADDRESS, state.leftForwardPercentage);

    EEPROM.put(state.LEFT_BACKWARD_PERCENTAGE_ADDRESS, state.leftBackwardPercentage);

    EEPROM.put(state.RIGHT_FORWARD_PERCENTAGE_ADDRESS, state.rightForwardPercentage);

    EEPROM.put(state.RIGHT_BACKWARD_PERCENTAGE_ADDRESS, state.rightBackwardPercentage);

    state.totalDistance = 0;
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
        // Serial.print("measureforward, right side = ");
        // Serial.println(right);

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

      // Serial.println(localClocked);
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
      driver.rightForward(0.4);
    }
    driver.rightBrake(L293D_BRAKE_TIME);

    // Reset circuits again just to make the function a "this gives us a fresh start" function
    encoder.reset();
    if(resetShifter) resetShifter();
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
    }
    return current; 
  }

  bool safe(HCSR04 &ears){
    constexpr uint8_t US_POLLING_RATE = 100;
    static uint8_t closeCount = 0, farCount = 0;
    constexpr uint8_t MAX_CLOSE_COUNT = 5;
    constexpr uint8_t MAX_FAR_COUNT = 5; 

    auto distance = ears.poll(US_POLLING_RATE);

    //sanity check to filter dodgy sensor readings
    if(distance <= 30){

      if(closeCount < MAX_CLOSE_COUNT){
        ++closeCount;
      } 
      farCount = 0;
      
    } else {

      if(farCount < MAX_FAR_COUNT){
        ++farCount;
      }
      closeCount = 0;
    }

    if(closeCount >= MAX_CLOSE_COUNT){
      return false;
    } else if(farCount >= MAX_FAR_COUNT){
      return true;
    }

    return true;
  }

}




