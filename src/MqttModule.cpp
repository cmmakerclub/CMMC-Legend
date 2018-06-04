#include "MqttModule.h"

void MqttModule::config(CMMC_System *os, AsyncWebServer* server) {
  Serial.println("MqttModule::config"); 
}; 

void MqttModule::setup() {
  Serial.println("MqttModule::once"); 

};

void MqttModule::loop() {
  Serial.println("MqttModule::loop"); 
} ;  
