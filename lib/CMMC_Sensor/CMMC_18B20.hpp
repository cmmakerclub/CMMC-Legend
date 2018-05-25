#include <Arduino.h>
#include <CMMC_Sensor.hpp>

class CMMC_18B20 : public CMMC_Sensor
{
public:
  typedef struct SENSOR_DATA
  {
    uint32_t temperature;
  };

  SENSOR_DATA data;
  OneWire *oneWire;
  DallasTemperature *sensor;
  
  void setup(int pin=0, int b=0)
  {
    oneWire = new OneWire(pin);
    sensor = new DallasTemperature(oneWire);
    DeviceAddress insideThermometer;

    sensor->begin();
    sensor->isParasitePowerMode();
    sensor->getAddress(insideThermometer, 0);
    sensor->setResolution(insideThermometer, 9);
    sensor->requestTemperatures();
    data.temperature = sensor->getTempC(insideThermometer);
  };

  void read()
  {
    static CMMC_18B20 *that = this;
    that->interval.every_ms(that->everyMs, []() {
      that->sensor->requestTemperatures();
      that->data.temperature = that->sensor->getTempCByIndex(0);
      that->cb((void *)&that->data, sizeof(that->data));
    });
  }
};
