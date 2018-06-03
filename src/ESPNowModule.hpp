#include <CMMC_Module.hpp>
#include <CMMC_ESPNow.h>
#include <CMMC_Utils.h>
#include <CMMC_SimplePair.h>

#ifndef CMMC_ESPNOW_MODULE_H
#define CMMC_ESPNOW_MODULE_H

class ESPNowModule: public CMMC_Module {
  public:
    void config(CMMC_System *os, AsyncWebServer* server) {
      strcpy(this->path, "/api/espnow");
      static ESPNowModule *that = this;
      this->_serverPtr = server;
      this->_managerPtr = new CMMC_Config_Manager("/espnow.json");
      this->_managerPtr->init();
      this->_managerPtr->load_config([](JsonObject * root, const char* content) {
        if (root == NULL) {
          Serial.print("espnow.json failed. >");
          Serial.println(content);
          return ;
        }
        else {
          Serial.println("[user] json loaded..");
          Serial.println(content);
          if (root->containsKey("mac")) {
            String macStr = String((*root)["mac"].as<const char*>());
            Serial.printf("Loaded mac %s\r\n", macStr.c_str());
            // CMMC::convertMacStringToUint8(macStr.c_str(), master_mac);
            // CMMC::printMacAddress(master_mac);
            Serial.println();
          }
          else {
            Serial.println("no mac field.");
          }
        }
      });
      this->configWebServer();
    }

    void once() {
      _init_espnow();
    }

    void loop() {

    }

  private:
    CMMC_ESPNow espNow;
    CMMC_SimplePair simplePair;
    uint8_t self_mac[6];
    bool sp_flag_done = false;
    void init_simple_pair() {
      simplePair.debug([](const char* msg) {
        Serial.println(msg);
      });
      static ESPNowModule *module = this;
      static bool *flag = &sp_flag_done;
      simplePair.begin(SLAVE_MODE, [](u8 status, u8 * sa, const u8 * data) { 
        Serial.println("evt_callback.");
        if (status == 0) {
          char buf[13];
          Serial.printf("[CSP_EVENT_SUCCESS] STATUS: %d\r\n", status);
          Serial.printf("WITH KEY: ");
          CMMC::dump(data, 16);
          Serial.printf("WITH MAC: ");
          CMMC::dump(sa, 6);
          CMMC::macByteToString(data, buf);
          CMMC::printMacAddress((uint8_t*)buf);
          module->_managerPtr->add_field("mac", buf);
          module->_managerPtr->commit();
          Serial.println("DONE...");
          *flag = true;
        }
        else {
          Serial.printf("[CSP_EVENT_ERROR] %d: %s\r\n", status, (const char*)data);
        }
      });

      simplePair.start();
      uint32_t startMs = millis();
      while (!sp_flag_done && (millis() - startMs < 10000)) {
        // led.toggle();
        Serial.println("waiting sp_flag_done ..");
        delay(1000L + (250 * sp_flag_done));
      }
      if (sp_flag_done) {
        Serial.println("pair done.");
      }
      else {
        Serial.println("do simple pair device not found.");
      }
    }

    void _init_espnow() {
      uint8_t* slave_addr = CMMC::getESPNowSlaveMacAddress();
      memcpy(self_mac, slave_addr, 6);
      Serial.print("Slave Mac Address: ");
      CMMC::printMacAddress(self_mac, true);

      pinMode(13, INPUT_PULLUP); 
      if (digitalRead(13) == 0) {
        init_simple_pair(); 
      } else {
        espNow.init(NOW_MODE_SLAVE);
        espNow.on_message_sent([](uint8_t *macaddr, u8 status) {
          // led.toggle();
          Serial.println(millis());
          Serial.printf("sent status %lu\r\n", status);
        });

        espNow.on_message_recv([](uint8_t * macaddr, uint8_t * data, uint8_t len) {
          // led.toggle();
          Serial.printf("GOT sleepTime = %lu\r\n", data[0]);
          // if (data[0] == 0)
          //   data[0] = DEFAULT_DEEP_SLEEP_M;
          // goSleep(data[0]);
        }); 
      }
    }
};

#endif