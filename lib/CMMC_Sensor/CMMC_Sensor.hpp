#ifndef CMMC_SENSOR_H
#define CMMC_SENSOR_H
//abstract class Sensor
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Adafruit_BME680.h"
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CMMC_Interval.hpp>
class CMMC_Sensor {
  public:
    String tag;
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
