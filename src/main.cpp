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
char myName[40];

// END MQTT CONNECTOR 

enum MODE{SETUP, RUN};

MODE mode;

bool flag_busy = false;
bool flag_mqtt_available = false;
CMMC_Blink *blinker;

CMMC_Config_Manager mqttConfigManager; 
CMMC_Config_Manager wifiConfigManager;

const char* http_username = "admin";
const char* http_password = "admin";

char sta_ssid[30] = "Nat";
char sta_pwd[30] = "espertap";

char ap_ssid[30] = "CMMC-Legend";
char ap_pwd[30] = ""; 

char mqtt_host[40] = "";
char mqtt_user[40] = "";
char mqtt_pass[40] = "";
char mqtt_clientId[40] = "";
char mqtt_port[10] = "";
char mqtt_device_name[15] = "";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
CMMC_Interval interval; 
//const char* hostName = "CMMC-Legend";

void init_sta() {
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  delay(20); 
  WiFi.mode(WIFI_STA);
  WiFi.hostname(ap_ssid);
  WiFi.begin(sta_ssid, sta_pwd); 
  digitalWrite(LED_BUILTIN, HIGH); 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", sta_ssid, sta_pwd);
    delay(300);
  } 
}

void init_ap() { 
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  delay(20); 
}

void init_userconfig() { 
  // mqttConfigManager.add_debug_listener([](const char* msg) {
  //   Serial.printf(">> %s \r\n", msg);
  // }); 
  wifiConfigManager.init("/wifi.json");
  mqttConfigManager.init("/mymqtt.json");

  wifiConfigManager.load_config([](JsonObject * root, const char* content) {
    Serial.println("[user] wifi config json loaded..");
    Serial.println(content); 
    if ((*root)["ap_ssid"] == NULL) return;
    strcpy(ap_ssid, (*root)["ap_ssid"]);
    strcpy(ap_pwd, (*root)["ap_password"]); 
    strcpy(sta_ssid, (*root)["sta_ssid"]); 
    strcpy(sta_pwd, (*root)["sta_password"]); 
  });

  Serial.printf("initializing SPIFFS ...");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("FS File: %s, size: %s", fileName.c_str(), String(fileSize).c_str());
  }

  mqttConfigManager.load_config([](JsonObject * root, const char* content) { 
    Serial.println("[user] mqtt config json loaded..");
    // Serial.println(content); 
    const char* host = (*root)["host"];
    const char* username = (*root)["username"];
    const char* password = (*root)["password"];
    const char* client_id = (*root)["clientId"];
    const char* port = (*root)["port"];
    const char* device_name = (*root)["deviceName"];

    if (host != NULL) {
      strcpy(mqtt_host, host);
      strcpy(mqtt_user, username);
      strcpy(mqtt_pass, password);
      strcpy(mqtt_clientId, client_id);
      strcpy(mqtt_port, port);
      strcpy(mqtt_device_name, device_name);

      MQTT_HOST = String(mqtt_host);
      MQTT_USERNAME = String(mqtt_user);
      MQTT_PASSWORD = String(mqtt_pass);
      MQTT_CLIENT_ID = String(mqtt_clientId);
      MQTT_PORT = String(mqtt_port).toInt();
      DEVICE_NAME = String(mqtt_device_name);
      MQTT_HOST = String(host);

      Serial.printf("host = %s port =%s, username = %s, password = %s, clientId = %s, deviceName = %s", 
        host, port, username, password, client_id, device_name);
      Serial.println();
      flag_mqtt_available = false;
    }
  });
}

void setup() {
  SPIFFS.begin();
  delay(10);

  init_userconfig(); 

  pinMode(0, OUTPUT);
  blinker = new CMMC_Blink; 
  blinker->init();
  digitalWrite(0, HIGH); 
  pinMode(0, INPUT_PULLUP);
  Serial.begin(57600);
  Serial.setDebugOutput(true); 
  blinker->blink(500, 2);
  // Serial.printf("initializing SPIFFS ...");
  // Dir dir = SPIFFS.openDir("/");
  // Serial.print("dir = ");
  // while (dir.next()) {
  //   String fileName = dir.fileName();
  //   size_t fileSize = dir.fileSize();
  //   Serial.printf("FS File: %s, size: %s\r\n", fileName.c_str(), String(fileSize).c_str());
  // } 
  if (!SPIFFS.exists("/enabled")) {
    blinker->blink(50, 2);
    Serial.println("AP Only Mode.");  
    mode = SETUP;
    Serial.printf("ESP8266 Chip id = %08X\n", ESP.getChipId());
    sprintf(&ap_ssid[5], "%08x", ESP.getChipId());
    init_ap();
    WiFi.softAP(ap_ssid, &ap_ssid[5]);
    setupWebServer(); 
  }
  else {
    mode = RUN;
    init_sta(); 
    Serial.println("WiFi Connected."); 
    blinker->blink(5000, 2);
    init_mqtt(); 
  } 
}

void loop() {
  if (mode == RUN) {
    mqtt->loop(); 
  } 

  uint32_t prev = millis();
  while(digitalRead(0) == LOW) {
    delay(50); 
    if (millis() - prev > 2000L) {
      Serial.println("LONG PRESSED.");
      blinker->blink(50); 
      while(digitalRead(0) == LOW) {
        delay(10); 
      }
      SPIFFS.remove("/enabled");
      ESP.restart();
    }
  }
}

