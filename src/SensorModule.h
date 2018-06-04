#include <CMMC_Module.h>

#ifndef CMMC_SENSOR_MODULE_H
#define CMMC_SENSOR_MODULE_H 

class SensorModule: public CMMC_Module {
  public:
    void config(CMMC_System *os, AsyncWebServer* server); 
    void setup();
    void loop(); 
  protected:
    void configWebServer();
  private:
};

#endif