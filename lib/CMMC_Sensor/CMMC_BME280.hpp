#include <Arduino.h>
#include <CMMC_Sensor.hpp>

class CMMC_BME280 : public CMMC_Sensor
{
private:
  Adafruit_BME280 *bme; // I2C
public:
  typedef struct SENSOR_DATA
  {
    uint32_t temperature;
    uint32_t humidity;
    uint32_t altitude;
    uint32_t pressure;
  };

  SENSOR_DATA data;

  void setup(int a=0, int b=0 )
  {
    this->tag = "BME280";
    bme = new Adafruit_BME280;
    if (!bme->begin())
    {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
    }
  };

  void read()
  {
    static CMMC_BME280 *that = this;
    that->interval.every_ms(that->everyMs, []() {
      that->data.temperature = that->bme->readTemperature();
      that->data.humidity = that->bme->readHumidity();
      that->data.pressure = that->bme->readPressure() / 100.0f;
      that->data.altitude = that->bme->readAltitude(SEALEVELPRESSURE_HPA);
      that->cb((void *)&that->data, sizeof(that->data));
    });
  };
};