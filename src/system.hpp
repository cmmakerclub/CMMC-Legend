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
#include <CMMC_Sensor.hpp>
#include <vector>
#include "gpio.hpp"
#include "CMMC_Module.hpp"

// CMMC_Sensor *sensorInstance;
CMMC_Gpio gpio;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
extern void setupWebServer(AsyncWebServer *, AsyncWebSocket *, AsyncEventSource *); 
enum MODE {SETUP, RUN};

std::vector<CMMC_Module*> _modules; 

uint32_t lastRecv;
CMMC_Interval interval;
CMMC_Blink *blinker;
CMMC_SENSOR_DATA_T sensorData;

void readSensorCb(void *d, size_t len)
{
  memcpy(&sensorData, d, len);
  Serial.printf("field1 %lu, field2 %lu \r\n", sensorData.field1, sensorData.field2);
};


class CMMC_Legend: public CMMC_System {
    MODE mode;

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
      Serial.println("Initializing GPIO..");
      pinMode(0, INPUT_PULLUP);
      blinker = new CMMC_Blink;
      blinker->init();
      blinker->setPin(2);
      gpio.setup();
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
      }
      else if (mode == RUN) {
        lastRecv = millis();
        blinker->blink(4000);
        int size = _modules.size();
        for (int i = 0 ; i < size; i++) {
          Serial.printf("call once idx = %d\r\n", i);
          _modules[i]->once();
        }
      }
    }

    void run() {
      static CMMC_Legend *that = this;
      // interval.every_ms(10L * 1000, []() { Serial.printf("Last Recv %lus ago.\r\n", ((millis() - lastRecv) / 1000)); });
      int size = _modules.size();
      for (int i = 0 ; i < size; i++) {
        _modules[i]->loop();
      }
      isLongPressed();
      yield();
    }

    void isLongPressed() {
      uint32_t prev = millis();
      while (digitalRead(0) == LOW) {
        delay(50);
        if ( (millis() - prev) > 5L * 1000L) {
          Serial.println("LONG PRESSED.");
          blinker->blink(50);
          while (digitalRead(0) == LOW) {
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