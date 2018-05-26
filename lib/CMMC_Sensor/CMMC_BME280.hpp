#include <Arduino.h>
#include <CMMC_Sensor.hpp>

class CMMC_BME280 : public CMMC_Sensor
{
private:
  Adafruit_BME280 *bme; // I2C
public:
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
      that->data.field1 = that->bme->readTemperature();
      that->data.field2 = that->bme->readHumidity();
      that->data.field3 = that->bme->readPressure() / 100.0f;
      that->data.field4 = that->bme->readAltitude(SEALEVELPRESSURE_HPA);
      that->cb((void *)&that->data, sizeof(that->data));
    });
  };
};