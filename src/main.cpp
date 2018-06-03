#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h> 
#include "version.h"
#include "system.hpp"
#include "CMMC_Module.hpp"

// MQTT CONNECTOR
char myName[40];

bool flag_restart = false; 
CMMC_Legend os;

class WiFiModule: public CMMC_Module {
  void config(CMMC_System *os, const AsyncWebServer* server) { 
    const char* _path = "/api/wifi/config"; 
    this->_managerPtr = new CMMC_Config_Manager("wifi.json");
    this->_managerPtr->init();
    this->_managerPtr->load_config([](JsonObject* json, const char* content) {
      Serial.println(content); 
    });
  } 

  void once() { 
    _init_sta();
  }

  void loop() { 
  }

  private:
  void _init_sta() {
    WiFi.softAPdisconnect();
    WiFi.disconnect();
    delay(20);
    WiFi.mode(WIFI_STA);
    delay(20);
    WiFi.hostname(ap_ssid);
    delay(20);
    WiFi.begin(sta_ssid, sta_pwd);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.printf ("Connecting to %s:%s\r\n", sta_ssid, sta_pwd);
      // os->isLongPressed();
      delay(300);
    }
    Serial.println("WiFi Connected.");
  } 

};

void setup()
{
  os.addModule(new WiFiModule());
  os.setup(); 
  Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop() 
{
  os.run();
}