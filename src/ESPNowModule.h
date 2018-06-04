#include <CMMC_Module.hpp>
#include <CMMC_ESPNow.h>
#include <CMMC_Utils.h>
#include <CMMC_SimplePair.h>
#include <CMMC_Sensor.hpp>
#include <CMMC_BME680.hpp>

#ifndef CMMC_ESPNOW_MODULE_H
#define CMMC_ESPNOW_MODULE_H

#define BUTTON_PIN  0

static CMMC_SENSOR_DATA_T data1;
static CMMC_SENSOR_DATA_T data2;

class ESPNowModule: public CMMC_Module {
  CMMC_Sensor *sensor1;
  public:
    void config(CMMC_System *os, AsyncWebServer* server); 
    void once(); 
    void loop(); 
    void setup(); 
  private:
    CMMC_System *os;
    CMMC_ESPNow espNow;
    CMMC_SimplePair simplePair;
    uint8_t self_mac[6];
    uint8_t master_mac[6];
    bool sp_flag_done = false; 
    void readSensor();
    void _init_simple_pair(); 
    void goSleep(uint32_t deepSleepM); 
    void _init_espnow();
}; 
#endif