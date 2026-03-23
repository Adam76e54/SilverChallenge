#pragma once

#include <Arduino.h>
#include <WiFiS3.h>
#include "State.h"
#include "Buffer.h"
#include "Commands.h"
#include "NetworkSetup.h"

constexpr uint8_t N = 200;
extern Buffer<N> in;
extern WiFiServer server;
extern WiFiClient GUI;

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
void handle(Buffer<N>& buffer){
  if(buffer.available() > 0){
    mappingHandle(buffer);
  }
}

template <uint8_t N>
void mappingHandle(Buffer<N>& buffer){
  char command[N];

  Serial.println("Entered mapping handle");

  if(buffer.read(command, N) == 0){
    return;
  }

  Serial.println(command);

  // Serial.println();

  if(command == nullptr) return;

  const char* value = &command[2];
  char function = command[0]; 

  Serial.println(function);

  if (function == comm::FORWARD){
    state.activity = FORWARD;
    Serial.println("Changed to forward");
  }
  else if (function == comm::TURN_LEFT){
    state.activity = LEFT;
    Serial.println("Changed to left");

  }
  else if (function == comm::TURN_RIGHT){
    state.activity = RIGHT;
    Serial.println("Changed to right");

  } 
  else if (function == comm::MATCH_WHEELS){
    state.activity = MATCHING;
  } 
  else if(function == comm::CALIBRATE_LEFT){
    state.activity = CALIBRATING_LEFT;
  }
  else if(function == comm::CALIBRATE_RIGHT){
    state.activity = CALIBRATING_RIGHT;
  }  
  else if(function == comm::SAVE_EEPROM){
    state.activity = SAVING_EEPROM;
  }
  else if(function == comm::CHANGE_TARGET){
    if(command[1] == comm::DELIMITER){
      state.targetDistance = atof(value);
    }

  } 
  else if(function == comm::LEFT_SPEED){
    if(command[1] == comm::DELIMITER){
      state.leftForwardPercentage = atof(value);
    Serial.print("Changed left to "); Serial.println(state.leftForwardPercentage);

    }
  }
  else if(function == comm::RIGHT_SPEED){
    if(command[1] == comm::DELIMITER){
      state.rightForwardPercentage = atof(value);
    }
  }
  else if(function == comm::RIGHT_FACTOR){
    if(command[1] == comm::DELIMITER){
      state.rightTurnFactor = atof(value);
    }
  }
  else if(function == comm::LEFT_FACTOR){
    if(command[1] == comm::DELIMITER){
      state.leftTurnFactor = atof(value);
    }
  }
  else if(function == comm::KP){
    if(command[1] == comm::DELIMITER){
      state.kp = atof(value);
    }
  }
  else if(function == comm::KI){
    if(command[1] == comm::DELIMITER){
      state.ki = atof(value);
    }
  }
  else if(function == comm::KD){
    if(command[1] == comm::DELIMITER){
      state.kd = atof(value);
    }
  }
}

void sendEvent(WiFiClient& GUI, const char* event){
  if(GUI && GUI.connected()){
    char message[128];

    int n = snprintf(message, sizeof(message), "%c%c%s", comm::EVENT, comm::DELIMITER, event);

    if(n > 0 && n < (int)sizeof(message)){
      GUI.println(message);
    }

    // GUI.print(comm::EVENT);
    // GUI.print(comm::DELIMITER);
    // GUI.println(message);
  }
}

void sendEvent(WiFiClient& GUI, const char* event, float num){
  if(GUI && GUI.connected()){
    char message[128];
    char numberAsString[16];

    dtostrf(num, 8, 2, numberAsString); // apparently %f printf() is disabled by default and this is preferred, no idea why

    int n = snprintf(message, sizeof(message), "%c%c%s%c%s",
      comm::EVENT, comm::DELIMITER, event, comm::DELIMITER, numberAsString);

    if(n > 0 && n < (int)sizeof(message)){
      GUI.println(message);
    }

    // GUI.print(comm::EVENT);
    // GUI.print(comm::DELIMITER);
    // GUI.print(message);
    // GUI.println(num);
  }
}