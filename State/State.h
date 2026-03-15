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
  float leftSpeedPercentage = 0;
  float rightSpeedPercentage = 0;

  const uint8_t EEPROM_SIZE = 16;
  const uint8_t LEFT_EEPROM_ADDRESS = 0;
  const uint8_t RIGHT_EEPROM_ADDRESS = LEFT_EEPROM_ADDRESS + sizeof(float);

  float targetCmPerSecond = 20;
  float offset = 0.18;

  float maxDistance = 15.0;
  float currentDistance = 999.0;

  bool stopped = true;


} state;

struct Coefficients{
  float KP, KI, KD;
} coefficients;