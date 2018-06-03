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
  public:
    void config(CMMC_System *os, AsyncWebServer* server) {
      strcpy(this->path, "/api/wifi/sta");
      static WiFiModule *that = this;
      this->_serverPtr = server;
      this->_managerPtr = new CMMC_Config_Manager("wifi.json");
      this->_managerPtr->init();
      this->_managerPtr->load_config([](JsonObject * root, const char* content) {
        if (root == NULL) {
          Serial.print("wifi.json failed. >");
          Serial.println(content);
          return ;
        }
        Serial.println("[user] wifi config json loaded..");
        const char* sta_config[2];
        sta_config[0] = (*root)["sta_ssid"];
        sta_config[1] = (*root)["sta_password"];
        if ((sta_config[0] == NULL) || (sta_config[1] == NULL)) {
          Serial.println("NULL..");
          SPIFFS.remove("/enabled");
          return;
        };
        strcpy(that->sta_ssid, sta_config[0]);
        strcpy(that->sta_pwd, sta_config[1]);
      });
      this->configWebServer();
    }

    void once() {
      _init_sta();
    }

    void loop() {

    }

  private:
    char sta_ssid[30] = "";
    char sta_pwd[30] = "";
    char ap_pwd[30] = "";
    void _init_sta() {
      WiFi.softAPdisconnect();
      WiFi.disconnect();
      delay(20);
      WiFi.mode(WIFI_STA);
      delay(20);
      // WiFi.hostname(ap_ssid);
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
  // os.addModule(new WiFiModule());
  os.setup();
  Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop()
{
  os.run();
}