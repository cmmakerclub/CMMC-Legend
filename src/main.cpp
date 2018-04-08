#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <CMMC_Blink.hpp>
#include <CMMC_Interval.hpp>
#include <CMMC_Config_Manager.h>
#include "ESP8266WiFi.h"
#include "webserver.h"

bool flag_busy = false;
bool flag_needs_commit = false;
CMMC_Blink *blinker;

CMMC_Config_Manager configManager; 
CMMC_Config_Manager mqttConfigManager; 

const char* http_username = "admin";
const char* http_password = "admin";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
CMMC_Interval interval;

const char* hostName = "CMMC-Legend";
String output;

void scanAndUpdateSSIDoutput();
void setup() {
  SPIFFS.begin();
  blinker = new CMMC_Blink;
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  blinker->init();
  blinker->blink(500, 2);
  Serial.begin(57600);

  Serial.setDebugOutput(true);
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  delay(20);

Serial.println("HELLO........");
  mqttConfigManager.init("/mymqtt.json");
  configManager.init("/myconfig.json"); 

  // configManager.add_debug_listener([](const char* m) {
  //   Serial.println(m);
  // });

  // mqttConfigManager.add_debug_listener([](const char* m) {
  //   Serial.println(m);
  // });

  configManager.load_config([](JsonObject * root) {
    Serial.println("[user] json loaded..");
    root->printTo(Serial);
    Serial.println();
  });

  mqttConfigManager.load_config([](JsonObject * root) {
    Serial.println("[user] mqtt config json loaded..");
    // {"cid":"clientId-23ea30e1d7","h":"sdfsdf","port":"1111","pwd":"sdfsdfff","usr":"sdf"}config file size =85 
    char mqtt_host[30];
    char mqtt_user[30];
    char mqtt_pass[30];
    char mqtt_clientId[30];
    char mqtt_port[10];
     strcpy(mqtt_host, (*root)["h"]);
     strcpy(mqtt_user, (*root)["usr"]);
     strcpy(mqtt_pass, (*root)["pwd"]);
     strcpy(mqtt_clientId, (*root)["cid"]);
     strcpy(mqtt_port, (*root)["port"]);
    root->printTo(Serial);
    Serial.println();
  });


  WiFi.mode(WIFI_STA);
  scanAndUpdateSSIDoutput();
  WiFi.hostname(hostName);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(hostName);
  WiFi.begin("Nat", "espertap");

  setupWebServer();

}

void scanAndUpdateSSIDoutput() {
  int n = WiFi.scanNetworks();
  Serial.println(n);
  String currentSSID = WiFi.SSID();
  output = "[";
  for ( int i = 0; i < n; i++ ) {
    if (output != "[") output += ',';
    output += "{\"name\": ";
    output += "\"";
    output += WiFi.SSID(i);
    output += "\",";
    output += "\"rssi\": ";
    output += WiFi.RSSI(i);
    output += ",";
    output += "\"encryption\": ";
    output += WiFi.encryptionType(i);
    output += "}";
    yield();
  }
  output += "]";
  Serial.println("ssid list has been updated.");
}

void loop() {
  interval.every_ms(20L * 1000, []() {
    while (flag_busy) {
      delay(100); 
    }
    // scanAndUpdateSSIDoutput(); 
  });
  
  if (flag_needs_commit) {
    flag_needs_commit = false;
    flag_busy = true;
    Serial.println("be commited.");
    mqttConfigManager.commit(); 
    configManager.commit(); 
    flag_busy = false;
    Serial.println("fs commited.");
  }
  // put your main code here, to run repeatedly:
}

