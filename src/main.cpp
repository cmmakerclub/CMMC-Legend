#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ArduinoJson.h>
#include <MqttConnector.h>

#include "version.h" 
#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"
#include "_config.h"

#include "webserver.h"
#include "utils.hpp" 


// extern _config
extern String DEVICE_NAME;
extern String MQTT_HOST;
extern String MQTT_USERNAME;
extern String MQTT_PASSWORD;
extern String MQTT_CLIENT_ID;
extern String MQTT_PREFIX;

extern int    MQTT_PORT;
extern int PUBLISH_EVERY;
extern int MQTT_CONNECT_TIMEOUT;
extern bool MQTT_LWT;

// MQTT CONNECTOR
MqttConnector *mqtt; 
uint32_t lastRecv; 
char myName[40]; 

bool flag_mqtt_available = false; 


bool flag_restart = false; 

void setup() {
  init_gpio(); 
  init_userconfig(); 
  select_bootmode();
  Serial.printf("app version=%s\r\n", LEGEND_APP_VERSION); 
} 

void loop() { 
  run();
} 