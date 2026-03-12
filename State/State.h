
#pragma once

#include <Arduino.h> 

enum MODE : uint8_t {MANUAL, MAPPING};

struct State{
  MODE mode = MANUAL;

  // should be [-1.0, 1.0]
  float leftSpeedPercentage = 0;
  float rightSpeedPercentage = 0;

  float maxDistance = 15.0;
  float currentDistance = 999.0;

  bool stopped = true;


} state;

struct Coefficients{
  float KP, KI, KD;
} coefficients;