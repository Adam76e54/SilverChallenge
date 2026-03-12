#pragma once

#include <Arduino.h>
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

  read(command, N);

  // here is where the hashtable should get placed
}

template <uint8_t N>
void mappingHandle(Buffer<N> buffer){
  
}