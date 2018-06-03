#include <CMMC_Module.hpp>
#include <CMMC_ESPNow.h>

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
    void _init_espnow() { 
      espNow.enable_retries(true);
    } 
};

#endif