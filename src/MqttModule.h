#include <CMMC_Module.h> 
#ifndef CMMC_MQTT_MODULE_H
#define CMMC_MQTT_MODULE_H 

class MqttModule: public CMMC_Module {
  public:
    void config(CMMC_System *os, AsyncWebServer* server); 
    void once();
    void loop(); 
  private: 
};

#endif