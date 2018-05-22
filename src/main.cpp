#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <CMMC_Blink.hpp>
#include <CMMC_Interval.hpp>
#include <CMMC_Config_Manager.h>


#include "version.h"
#include "webserver.h"

#include <ArduinoJson.h>
#include <MqttConnector.h>
#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"
#include "_config.h"

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

// END MQTT CONNECTOR 

enum MODE{SETUP, RUN};

MODE mode;

bool flag_mqtt_available = false;
static CMMC_Blink *blinker;

CMMC_Config_Manager mqttConfigManager; 
CMMC_Config_Manager wifiConfigManager;

const char* http_username = "admin";
const char* http_password = "admin";

char sta_ssid[30] = "";
char sta_pwd[30] = "";

char ap_ssid[30] = "CMMC-Legend";
char ap_pwd[30] = ""; 

char mqtt_host[40] = "";
char mqtt_user[40] = "";
char mqtt_pass[40] = "";
char mqtt_clientId[40] = "";
char mqtt_prefix[40] = "";
char mqtt_port[10] = "";
char mqtt_device_name[15] = "";

bool flag_restart = false;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
CMMC_Interval interval; 
//const char* hostName = "CMMC-Legend";
void checkConfigMode(); 

#include "utils.hpp" 


void setup() {
  SPIFFS.begin();
  blinker = new CMMC_Blink; 
  blinker->init();
  blinker->setPin(2); 
  pinMode(0, INPUT_PULLUP); 
  Serial.begin(57600);
  blinker->blink(500); 
  delay(10);
  // Serial.setDebugOutput(true); 

  init_userconfig(); 
  select_bootmode();
  Serial.printf("app version=%s\r\n", LEGEND_APP_VERSION); 
}

void loop() { 
  run();
} 