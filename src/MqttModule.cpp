#include "MqttModule.h"

void MqttModule::config(CMMC_System *os, AsyncWebServer* server) {
  Serial.println("MqttModule::config"); 
}; 

void MqttModule::once() {
  Serial.println("MqttModule::once"); 
  while(1) {
    Serial.println("HELLO... from MqttModule");
    delay(1000);
  }
};

void MqttModule::loop() {
  Serial.println("MqttModule::loop"); 
} ;  
