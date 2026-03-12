#pragma once

#include <Arduino.h>
#include <WiFiS3.h>
#include "State.h"
#include "Buffer.h"

// Keep connection alive
void keep(WiFiClient& GUI, WiFiServer& server){
  if(!GUI){
    GUI = server.available();
  }
}

//read command into buffer
template <uint8_t N> 
bool read(WiFiClient& GUI, Buffer<N>& buffer){
  if(GUI && GUI.connected()){
    while(GUI.available() > 0){
      byte commandByte = GUI.read();
      buffer.write(commandByte);
      if(commandByte == '\n')break;
    }
  }
  return false;
}

template <uint8_t N>
void handle(Buffer<N> buffer){
  if(buffer.available() > 0){
    if(state.mode == MANUAL){
      manualHandle(buffer);
    } else if (state.mode == MAPPING){
      mappingHandle(buffer);
    }
  }
}

template <uint8_t N>
void manualHandle(Buffer<N> buffer){
  char command[N];

  buffer.read(command, N);

  if(command == nullptr) return;

  char function = *command; 

  if (function == *comm::FORWARD){
    state.leftSpeedPercentage = 0.8;
    state.rightSpeedPercentage = 1;
  }
  else if (function == *comm::TURN_LEFT){
    state.leftSpeedPercentage = 0.8;
    state.rightSpeedPercentage = -1;
  }
  else if (function == *comm::TURN_RIGHT){
    state.leftSpeedPercentage = -0.8;
    state.rightSpeedPercentage = 1;
  }
  else if (function == *comm::BACKWARD){
    state.leftSpeedPercentage = -0.8;
    state.rightSpeedPercentage = -1;
  }
}

template <uint8_t N>
void mappingHandle(Buffer<N> buffer){
  char command[N];

  buffer.read(command, N);

  if(command == nullptr || command[1] != comm::DELIMITER) return;

  uint8_t value = command[2];
  char function = *command; 
  
  if (function == *comm::FORWARD){
    state.activity = FORWARD;
    state.targetDistance = atof(value);
  }
  else if (function == *comm::TURN_LEFT){
    state.activity = LEFT;
    state.targetAngle = atoi(value)
  }
  else if (function == *comm::TURN_RIGHT){
    state.activity = RIGHT;
    state.targetAngle = atoi(value)
  } 
}