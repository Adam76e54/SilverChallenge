#pragma once

#include <Arduino.h> 

enum MODE : uint8_t {MANUAL, MAPPING};
enum ACTIVITY : uint8_t {IDLE, FORWARD, LEFT, RIGHT, MATCHING, CALIBRATING_RIGHT, CALIBRATING_LEFT, SAVING_EEPROM};

struct State{
  MODE mode = MANUAL;
  ACTIVITY activity = IDLE; 

  float targetDistance = 0;

  float totalDistance = 0;

  /*
    - tolerance
    - offset on right, left, 40 cm, 30 cm, 20 cm
    - offset on actual wheel speeds too
    - current speed
  */
  // should be [-1.0, 1.0]
  float leftForwardPercentage = 0;
  float rightForwardPercentage = 0;

  float leftBackwardPercentage = 0;
  float rightBackwardPercentage = 0;

  float leftForwardCmPerSecond = 0; 
  float rightForwardCmPerSecond = 0;

  float leftBackwardCmPerSecond = 0;
  float rightBackwardCmPerSecond = 0;

  float rightTurnFactor = 1.0f;
  float leftTurnFactor = 1.0f;

  float kp = 0.05;
  float ki = 0.0;
  float kd = 0.0;

  unsigned long leftTurnTime = 0;
  unsigned long righTurnTime = 0;

  const unsigned int LEFT_FORWARD_PERCENTAGE_ADDRESS = 0;
  const unsigned int RIGHT_FORWARD_PERCENTAGE_ADDRESS = LEFT_FORWARD_PERCENTAGE_ADDRESS + sizeof(float);

  const unsigned int LEFT_FORWARD_CM_PER_SECOND_ADDRESS = RIGHT_FORWARD_PERCENTAGE_ADDRESS + sizeof(float);
  const unsigned int RIGHT_FORWARD_CM_PER_SECOND_ADDRESS = LEFT_FORWARD_CM_PER_SECOND_ADDRESS + sizeof(float);
  
  const unsigned int LEFT_BACKWARD_PERCENTAGE_ADDRESS = RIGHT_FORWARD_CM_PER_SECOND_ADDRESS + sizeof(float);
  const unsigned int RIGHT_BACKWARD_PERCENTAGE_ADDRESS = LEFT_BACKWARD_PERCENTAGE_ADDRESS + sizeof(float);

  const unsigned int LEFT_BACKWARD_CM_PER_SECOND_ADDRESS = RIGHT_BACKWARD_PERCENTAGE_ADDRESS + sizeof(float);
  const unsigned int RIGHT_BACKWARD_CM_PER_SECOND_ADDRESS = LEFT_BACKWARD_CM_PER_SECOND_ADDRESS + sizeof(float);

  const unsigned int LEFT_TURN_TIME_ADDRESS = RIGHT_BACKWARD_CM_PER_SECOND_ADDRESS + sizeof(float);
  const unsigned int RIGHT_TURN_TIME_ADDRESS = LEFT_TURN_TIME_ADDRESS + sizeof(unsigned long);

  float currentDistance = 999.0;

  bool stopped = true;


} state;