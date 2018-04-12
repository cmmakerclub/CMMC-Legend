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
int relayPin = 15; 
int relayPinState = HIGH;
char myName[40];

// END MQTT CONNECTOR 

bool flag_busy = false;
bool flag_needs_commit = false;
bool flag_needs_scan_wifi = true;
bool flag_load_mqtt_config = false;
bool flag_load_wifi_config = false;
CMMC_Blink *blinker;

CMMC_Config_Manager wifiConfigManager;
CMMC_Config_Manager mqttConfigManager; 

const char* http_username = "admin";
const char* http_password = "admin";

char sta_ssid[30] = "CMMC-3rd";
char sta_pwd[30] = "espertap";

char ap_ssid[30] = "CMMC-Legend";
char ap_pwd[30] = "";

char wifi_config_json[120];

//char mqtt_host[30];
//char mqtt_user[30];
//char mqtt_pass[30];
//char mqtt_clientId[30];
//char mqtt_port[10];
//char mqtt_device_name[15];
char mqtt_config_json[120];


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
CMMC_Interval interval;

//const char* hostName = "CMMC-Legend";
String wifi_list_json;

void scanAndUpdateSSIDoutput();
void init_wifi() { 
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  delay(20); 
  WiFi.mode(WIFI_STA);
  scanAndUpdateSSIDoutput();
  WiFi.hostname(ap_ssid);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_pwd); 
  WiFi.begin(sta_ssid, sta_pwd);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", sta_ssid, sta_pwd);
    delay(300);
  } 
  Serial.println("WiFi Connected.");
  digitalWrite(LED_BUILTIN, HIGH);
}

void init_userconfig() {

  mqttConfigManager.init("/mymqtt.json");
  wifiConfigManager.init("/wifi.json");

  wifiConfigManager.load_config([](JsonObject * root, const char* content) {
    Serial.println("[user] wifi config json loaded..");
    Serial.println(content);
    strcpy(wifi_config_json, content);

    const char* m_ap_ssid = (*root)["ap_ssid"];
    const char* m_ap_pwd = (*root)["ap_pwd"];
    const char* m_sta_ssid = (*root)["sta_ssid"];
    const char* m_sta_password = (*root)["sta_password"];
    const char* m_sta_manual_ssid = (*root)["sta_manual_ssid"];

    if (m_ap_ssid != NULL) {
        strcpy(ap_ssid, m_ap_ssid);
    }

    if (m_ap_pwd != NULL) {
        strcpy(ap_pwd, m_ap_pwd);
    }

    if (m_sta_manual_ssid != NULL) {
        strcpy(sta_ssid, m_sta_manual_ssid);
    } else {
        if (m_sta_ssid != NULL) {
            strcpy(sta_ssid, m_sta_ssid);
        }
    }

    if (m_sta_password != NULL) {
        strcpy(sta_pwd, m_sta_password);
    }
    
  });

  mqttConfigManager.load_config([](JsonObject * root, const char* content) { 
    Serial.println("[user] mqtt config json loaded..");
    Serial.println(content); 
    strcpy(mqtt_config_json, content); 
    const char* host = (*root)["host"];
    const char* username = (*root)["username"];
    const char* password = (*root)["password"];
    const char* client_id = (*root)["clientId"];
    const char* port = (*root)["port"];
    const char* device_name = (*root)["deviceName"];
    const char* lwt = (*root)["lwt"];

    if (host != NULL) {
//      strcpy(mqtt_host, host);
//      strcpy(mqtt_user, username);
//      strcpy(mqtt_pass, password);
//      strcpy(mqtt_clientId, client_id);
//      strcpy(mqtt_port, port);
//      strcpy(mqtt_device_name, device_name);

      if (String(lwt).toInt() == 0) {
        MQTT_LWT = true;
      } else {
        MQTT_LWT = false;
      }

//      MQTT_HOST = String(mqtt_host);
//      MQTT_USERNAME = String(mqtt_user);
//      MQTT_PASSWORD = String(mqtt_pass);
//      MQTT_CLIENT_ID = String(mqtt_clientId);
//      MQTT_PORT = String(mqtt_port).toInt();
//      DEVICE_NAME = String(mqtt_device_name);
      MQTT_HOST = String(host);
      MQTT_USERNAME = String(username);
      MQTT_PASSWORD = String(password);
      MQTT_CLIENT_ID = String(client_id);
      MQTT_PORT = String(port).toInt();
      DEVICE_NAME = String(device_name);
      MQTT_LWT = String(lwt);

      Serial.printf("host = %s port =%s, username = %s, password = %s, clientId = %s, deviceName = $s, lwt = %s", host, port, username, password, client_id, device_name, lwt);
      Serial.println(); 
    }
  });
}

void setup() {
  SPIFFS.begin();
  blinker = new CMMC_Blink;
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  blinker->init();
  blinker->blink(500, 2);
  Serial.begin(57600);
  Serial.setDebugOutput(true);

  // Relay OUTPUT
  pinMode(relayPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(relayPin, relayPinState);;


  init_userconfig();
  init_wifi();
  init_mqtt(); 
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
  mqtt->loop();

   interval.every_ms(30L * 1000, []() {
     while (flag_busy) {
       delay(100);
     }
   });

  if (flag_load_wifi_config) {
    wifiConfigManager.load_config([](JsonObject * root, const char* content) { 
      Serial.println("[user] wifi config json loaded..");
      Serial.println(content); 
      strcpy(wifi_config_json, content); 
    });
    flag_load_wifi_config = false;
  }

  if (flag_load_mqtt_config) {
    mqttConfigManager.load_config([](JsonObject * root, const char* content) { 
      Serial.println("[user] mqtt config json loaded..");
      Serial.println(content); 
      strcpy(mqtt_config_json, content); 
    });
    flag_load_mqtt_config = false;
  } 
}

