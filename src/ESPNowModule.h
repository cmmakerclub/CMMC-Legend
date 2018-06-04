
#ifndef CMMC_ESPNOW_MODULE_H
#define CMMC_ESPNOW_MODULE_H
#define CMMC_USE_ALIAS 

#include <CMMC_Utils.h>
#include <CMMC_Module.h>
#include <CMMC_ESPNow.h>
#include <CMMC_SimplePair.h>
#include <CMMC_Sensor.hpp>
#include <CMMC_LED.h>
#include <CMMC_BME680.hpp>


#define BUTTON_PIN  0

static CMMC_SENSOR_DATA_T data1;
static CMMC_SENSOR_DATA_T data2;

class ESPNowModule: public CMMC_Module {
  CMMC_Sensor *sensor1;
  public:
    void config(CMMC_System *os, AsyncWebServer* server); 
    void loop(); 
    void configLoop(); 
    void setup(); 
  private:
    CMMC_System *os;
    CMMC_ESPNow espNow;
    CMMC_SimplePair simplePair;
    CMMC_LED *led;
    uint8_t self_mac[6];
    uint8_t master_mac[6]; 
    bool sp_flag_done = false; 
    void _read_sensor();
    void _init_simple_pair(); 
    void _go_sleep(uint32_t deepSleepM); 
    void _init_espnow();
}; 
#endif