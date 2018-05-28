#ifndef CMMC_SENSOR_H
#define CMMC_SENSOR_H
//abstract class Sensor
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CMMC_Interval.hpp>
  typedef struct __attribute((__packed__)) {
    uint8_t from[6];
    uint8_t to[6];
    uint8_t type = 0;
    uint32_t battery = 0x00;
    uint32_t field1 = 0x00;
    uint32_t field2 = 0x00;
    uint32_t field3 = 0x00;
    uint32_t field4 = 0x00;
    uint32_t field5 = 0x00;
    uint32_t field6 = 0x00;
    uint8_t nameLen = 0x00;
    char sensorName[16];
    uint32_t ms = 0;
    uint32_t sent_ms = 0;
    uint32_t sum = 0;
  } CMMC_SENSOR_DATA_T;
class CMMC_Sensor {
  public:

    String tag;
    CMMC_SENSOR_DATA_T data;
    CMMC_Sensor() {
      cb = [](void* d, uint32_t len) { 
        Serial.println("DUMMY Sensor CB.");
      };
    }
    typedef std::function<void(void *, size_t len)> callback_t;
    inline virtual void setup(int a = 0, int b = 0)  =0;
    virtual void read() = 0;
    void onData(callback_t cb) {
      this->cb = cb;
    }
    void every(uint32_t ms) {
      this->everyMs = ms;
    }
  protected:
    callback_t cb;
    CMMC_Interval interval;
    uint32_t everyMs = 5000L;
};
#endif
