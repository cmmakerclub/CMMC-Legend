#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <CMMC_Blink.hpp>
#include <CMMC_Interval.hpp>
#include <CMMC_Config_Manager.h>
#include "CMMC_System.hpp"
#include <vector>
#include "CMMC_Module.hpp"

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");
static AsyncEventSource events("/events"); 
static CMMC_Blink *blinker;

enum MODE {SETUP, RUN}; 
extern void setupWebServer(AsyncWebServer *, AsyncWebSocket *, AsyncEventSource *); 

class CMMC_Legend: public CMMC_System {
  public:
    void setup() {
      CMMC_System::setup();
    }
    void init_fs() {
      SPIFFS.begin();
      Dir dir = SPIFFS.openDir("/");
      Serial.println("--------------------------");
      while (dir.next()) {
        File f = dir.openFile("r");
        Serial.printf("> %s \r\n", dir.fileName().c_str());
      }
      /*******************************************
         Boot Mode Selection
       *******************************************/
      Serial.println("--------------------------");
      if (!SPIFFS.exists("/enabled")) {
        mode = SETUP;
      }
      else {
        mode = RUN;
      }
    }

    void init_gpio() {
      pinMode(13, INPUT_PULLUP);
      blinker = new CMMC_Blink;
      blinker->init();
      blinker->setPin(2);
      Serial.begin(57600);
      Serial.println();
      blinker->blink(500);
      delay(10);
    }

    void init_user_sensor() {
      Serial.printf("Initializing Sensor.. MODE=%s\r\n", mode == SETUP ? "SETUP" : "RUN");
      if (mode == SETUP) {
        return;
      }
    }

    void addModule(CMMC_Module* module) {
      _modules.push_back(module);
      Serial.printf("addModule.. size = %d\r\n", _modules.size());
    }

    void init_user_config() {
      Serial.println("Initializing ConfigManager files.");
    }

    void init_network() {
      Serial.println("Initializing network.");
      for (int i = 0 ; i < _modules.size(); i++) {
        Serial.printf("call config idx = %d\r\n", i);
        _modules[i]->config(this, &server);
      }
      if (mode == SETUP) {
        _init_ap();
        setupWebServer(&server, &ws, &events);
        blinker->blink(50);
        while(1) {
          for (int i = 0 ; i < _modules.size(); i++) {
            _modules[i]->setup();
          }
          yield();
        }
      }
      else if (mode == RUN) {
        blinker->blink(4000);
        for (int i = 0 ; i < _modules.size(); i++) {
          Serial.printf("call once idx = %d\r\n", i);
          _modules[i]->once();
        }
      }
    }

    CMMC_Blink *getBlinker() {
      return blinker;
    }

    void run() {
      static CMMC_Legend *that = this;
      int size = _modules.size();
      for (int i = 0 ; i < size; i++) {
        _modules[i]->loop();
      }
      isLongPressed();
      yield();
    }

    void isLongPressed() {
      uint32_t prev = millis();
      while (digitalRead(13) == LOW) {
        delay(50);
        if ( (millis() - prev) > 5L * 1000L) {
          Serial.println("LONG PRESSED.");
          blinker->blink(50);
          while (digitalRead(13) == LOW) {
            delay(10);
          }
          SPIFFS.remove("/enabled");
          Serial.println("being restarted.");
          delay(1000);
          ESP.restart();
        }
      }
    }

  private: 
    MODE mode;
    std::vector<CMMC_Module*> _modules; 

    char ap_ssid[30] = "CMMC-Legend";
    void _init_ap() {
      WiFi.disconnect();
      WiFi.softAPdisconnect();
      delay(10);
      WiFi.mode(WIFI_AP);
      delay(10);
      IPAddress Ip(192, 168, 4, 1);
      IPAddress NMask(255, 255, 255, 0);
      WiFi.softAPConfig(Ip, Ip, NMask);
      sprintf(&this->ap_ssid[5], "%08x", ESP.getChipId());
      WiFi.softAP(ap_ssid, &ap_ssid[5]);
      delay(20);
      IPAddress myIP = WiFi.softAPIP();
      Serial.println();
      Serial.print("AP IP address: ");
      Serial.println(myIP);
    }
};

#include "webserver.h"