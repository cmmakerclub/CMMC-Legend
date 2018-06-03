#ifndef CMMC_MODULE_H
#define CMMC_MODULE_H

#include <Arduino.h>
#include "CMMC_System.hpp"
#include <CMMC_Config_Manager.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>

class CMMC_Module {
  protected:
    char path[20];
    CMMC_Config_Manager *_managerPtr;
    AsyncWebServer *_serverPtr;
    void configWebServer() {
        // strcpy(this->path, path);
        Serial.println("init server");
        static CMMC_Module *that = this;
        static CMMC_Config_Manager *m = this->_managerPtr;
        _serverPtr->on(this->path, HTTP_POST, [](AsyncWebServerRequest *request) {
          String output = that->saveConfig(request, m);
          request->send(200, "application/json", output);
        }); 
    }
    String saveConfig(AsyncWebServerRequest *request, CMMC_Config_Manager* configManager) {
      int params = request->params();
      String output = "{";
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          const char* key = p->name().c_str();
          const char* value = p->value().c_str();
          String v;
          if (value == 0) {
            Serial.println("value is null..");
            v = String("");
          }
          else {
            v = String(value);
          }
          output += "\"" + String(key) + "\"";
          if (i == params - 1 ) {
            output += ":\"" + v + "\"";
          }
          else {
            output += ":\"" + v + "\",";
          }
          configManager->add_field(key, v.c_str());
        }
      }
      output += "}";
      configManager->commit();
      return output;
    }
  public:
    virtual void config(CMMC_System *os, AsyncWebServer* server) = 0; 
    virtual void once() = 0;
    virtual void loop() = 0;
};

#endif