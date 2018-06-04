#ifndef CMMC_MODULE_H
#define CMMC_MODULE_H

#include <Arduino.h>
#include "CMMC_System.hpp"
#include <CMMC_ConfigManager.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>

class CMMC_Module {
  protected:
    CMMC_ConfigManager *_managerPtr;
    AsyncWebServer *_serverPtr;
    char path[20];

    void configWebServer();
    String saveConfig(AsyncWebServerRequest *request, CMMC_ConfigManager* configManager);
  public:
    virtual void config(CMMC_System *os, AsyncWebServer* server) = 0; 
    virtual void once() = 0;
    virtual void loop() = 0;
    virtual void setup();
};

#endif