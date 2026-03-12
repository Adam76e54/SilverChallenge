#pragma once

#include <Arduino.h>
#include "Buffer.h"

void keep(WiFiClient& GUI, WiFiServer& server){
  if(!GUI){
    GUI = server.available();
  }
}