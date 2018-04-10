#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <CMMC_Blink.hpp>
#include <CMMC_Interval.hpp>
#include <CMMC_Config_Manager.h>
#include "webserver.h"

bool flag_busy = false;
bool flag_needs_commit = false;
bool flag_needs_scan_wifi = true;
CMMC_Blink *blinker;

CMMC_Config_Manager wifiConfigManager;
CMMC_Config_Manager mqttConfigManager; 

const char* http_username = "admin";
const char* http_password = "admin";

char ap_ssid[30] = "CMMC-Legend";
char ap_pwd[30] = "";

char wifi_config_json[120];

char mqtt_host[30];
char mqtt_user[30];
char mqtt_pass[30];
char mqtt_clientId[30];
char mqtt_port[10];

char mqtt_config_json[120];

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
CMMC_Interval interval;

//const char* hostName = "CMMC-Legend";
String wifi_list_json;

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

  mqttConfigManager.init("/mymqtt.json");
  wifiConfigManager.init("/wifi.json");

  // wifiConfigManager.add_debug_listener([](const char* m) {
  //   Serial.println(m);
  // });

  // mqttConfigManager.add_debug_listener([](const char* m) {
  //   Serial.println(m);
  // });

  wifiConfigManager.load_config([](JsonObject * root, const char* content) {
    Serial.println("[user] wifi config json loaded..");
    Serial.println(content);
//    root->printTo(Serial);
//    Serial.println();
    strcpy(wifi_config_json, content);

    const char* m_ap_ssid = (*root)["ap_ssid"];
    const char* m_ap_pwd = (*root)["ap_pwd"];

    if (m_ap_ssid != NULL) {
        strcpy(ap_ssid, m_ap_ssid);
    }

    if (m_ap_pwd != NULL) {
        strcpy(ap_pwd, m_ap_pwd);
    }

  });

  mqttConfigManager.load_config([](JsonObject * root, const char* content) { 
    Serial.println("[user] mqtt config json loaded..");
    Serial.println(content); 
    strcpy(mqtt_config_json, content); 
    const char* h = (*root)["h"];
    const char* u = (*root)["usr"];
    const char* pwd = (*root)["pwd"];
    const char* cid = (*root)["cid"];
    const char* port = (*root)["port"];

    if (h != NULL) {
      strcpy(mqtt_host, h);
      strcpy(mqtt_user, u);
      strcpy(mqtt_pass, pwd);
      strcpy(mqtt_clientId, cid);
      strcpy(mqtt_port, port); 
      Serial.printf("host = %s port =%s, user = %s, pass = %s, clientId = %s", h, port, u, pwd, cid);
      Serial.println(); 
    }
  });

  WiFi.mode(WIFI_STA);
  scanAndUpdateSSIDoutput();
  WiFi.hostname(ap_ssid);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_pwd);
  WiFi.begin("CMMC-3rd", "espertap");

  setupWebServer();

}

void scanAndUpdateSSIDoutput() {
  int n = WiFi.scanNetworks();
  Serial.println(n);
  String currentSSID = WiFi.SSID();
  wifi_list_json = "[";
  for ( int i = 0; i < n; i++ ) {
    if (wifi_list_json != "[") wifi_list_json += ',';
    wifi_list_json += "{\"name\": ";
    wifi_list_json += "\"";
    wifi_list_json += WiFi.SSID(i);
    wifi_list_json += "\",";
    wifi_list_json += "\"rssi\": ";
    wifi_list_json += WiFi.RSSI(i);
    wifi_list_json += ",";
    wifi_list_json += "\"encryption\": ";
    wifi_list_json += WiFi.encryptionType(i);
    wifi_list_json += "}";
    yield();
  }
  wifi_list_json += "]";
  Serial.println("ssid list has been updated.");
}

void loop() {
   interval.every_ms(30L * 1000, []() {
     while (flag_busy) {
       delay(100);
     }
   });

  // if (flag_needs_scan_wifi) {
  //   scanAndUpdateSSIDoutput(); 
  //   flag_needs_scan_wifi = false;
  // }
  
  if (flag_needs_commit) {
    flag_needs_commit = false;
    flag_busy = true;
    Serial.println("be commited.");
    mqttConfigManager.commit();
    delay(100);
    wifiConfigManager.commit(); 

    mqttConfigManager.load_config([](JsonObject * root, const char* content) { 
      Serial.println("[user] mqtt config json loaded..");
      Serial.println(content); 
      strcpy(mqtt_config_json, content); 
    });

    flag_busy = false;
    Serial.println("fs commited.");
  }
}

