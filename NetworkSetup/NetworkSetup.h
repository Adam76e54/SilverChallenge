#pragma once
#include <WiFiS3.h>
#include <Arduino.h>

namespace wifi{
  inline constexpr char SSID[] =  "X14_Network\0";
  inline constexpr char PASSWORD[] = "super_secret_password\0";
  inline constexpr uint16_t PORT = 80;

  void initialiseAccessPoint();

  void connect(const char* ssid, const char* password, IPAddress& address);

  
}