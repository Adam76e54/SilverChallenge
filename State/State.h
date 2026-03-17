#pragma once

#include <Arduino.h> 

enum MODE : uint8_t {MANUAL, MAPPING};
enum ACTIVITY : uint8_t {IDLE, FORWARD, LEFT, RIGHT};

struct State{
  MODE mode = MANUAL;
  ACTIVITY activity = IDLE; 

  float targetDistance = 0;
  int targetAngle = 0;

  float totalDistance = 0;

  // should be [-1.0, 1.0]
  float leftForwardPercentage = 0;
  float rightForwardPercentage = 0;

  float leftBackwardPercentage = 0;
  float rightBackwardPercentage = 0;

  float leftForwardCmPerSecond = 0; 
  float rightForwardCmPerSecond = 0;

  float leftBackwardCmPerSecond = 0;
  float rightBackwardCmPerSecond = 0;

  const uint8_t LEFT_FORWARD_PERCENTAGE_ADDRESS = 0;
  const uint8_t RIGHT_FORWARD_PERCENTAGE_ADDRESS = LEFT_FORWARD_PERCENTAGE_ADDRESS + sizeof(float);

  const uint8_t LEFT_FORWARD_CM_PER_SECOND_ADDRESS = RIGHT_FORWARD_PERCENTAGE_ADDRESS + sizeof(float);
  const uint8_t RIGHT_FORWARD_CM_PER_SECOND_ADDRESS = LEFT_FORWARD_CM_PER_SECOND_ADDRESS + sizeof(float);
  
  const uint8_t LEFT_BACKWARD_PERCENTAGE_ADDRESS = RIGHT_FORWARD_CM_PER_SECOND_ADDRESS + sizeof(float);
  const uint8_t RIGHT_BACKWARD_PERCENTAGE_ADDRESS = LEFT_BACKWARD_PERCENTAGE_ADDRESS + sizeof(float);

  const uint8_t LEFT_BACKWARD_CM_PER_SECOND_ADDRESS = RIGHT_BACKWARD_PERCENTAGE_ADDRESS + sizeof(float);
  const uint8_t RIGHT_BACKWARD_CM_PER_SECOND_ADDRESS = LEFT_BACKWARD_CM_PER_SECOND_ADDRESS + sizeof(float);

  float targetCmPerSecond = 20;
  float offset = 0.18;

  float maxDistance = 15.0;
  float currentDistance = 999.0;

  bool stopped = true;


} state;

struct Coefficients{
  float KP, KI, KD;
} coefficients;