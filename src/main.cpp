#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <CMMC_Blink.hpp>
#include <CMMC_Interval.hpp>
#include <CMMC_Config_Manager.h>
#include "ESP8266WiFi.h"

CMMC_Blink *blinker;
CMMC_Config_Manager configManager;

const char* http_username = "admin";
const char* http_password = "admin";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
CMMC_Interval interval;

#include "webserver.h"

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

  configManager.init("/myconfig.json");
  configManager.load_config([](JsonObject * root) {
    Serial.println("[user] json loaded..");
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
    scanAndUpdateSSIDoutput();
  });
  // put your main code here, to run repeatedly:
}

