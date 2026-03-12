#include "NetworkSetup.h"

namespace wifi{
  void initialiseAccessPoint(){
    Serial.print("Creating access point: ");
    Serial.println(SSID);

    int status = WiFi.beginAP(SSID, PASSWORD);

    if(status != WL_AP_LISTENING){
      Serial.println("Failed to create access point");
      while (true) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
      }
    }

    Serial.print("Access point IP address: ");
    Serial.println(WiFi.localIP());
  }

  void connect(const char* ssid, const char* password, IPAddress& address){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println();
    Serial.print("Connected and allocated IP: ");
    Serial.print(WiFi.localIP());
    address = WiFi.localIP();
  }

  
}