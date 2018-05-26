#include <Arduino.h>
#include <CMMC_Sensor.hpp>
#define SEALEVELPRESSURE_HPA (1013.25)

class CMMC_BME680 : public CMMC_Sensor
{
private:
  Adafruit_BME680 *bme; // I2C
public:
  CMMC_BME680() {
    this->tag = "BME680";
    bme = new Adafruit_BME680;
    Serial.println("680 constructure.");
  }
  ~CMMC_BME680() {
    Serial.println("680 constructure.");
    delete bme;
  }

  void setup(int a=0, int b=0)
  {
    Serial.println("BME680 begin..");
    if (!bme->begin())
    {
      Serial.println("Could not find a valid BME680 sensor, check wiring!");
    }
    else
    {
      Serial.println("BME680 initialized.");
      // Set up oversampling and filter initialization
      bme->setTemperatureOversampling(BME680_OS_8X);
      bme->setHumidityOversampling(BME680_OS_2X);
      bme->setPressureOversampling(BME680_OS_4X);
      bme->setIIRFilterSize(BME680_FILTER_SIZE_3);
      bme->setGasHeater(320, 150); // 320*C for 150 ms
    }
  };

  void read()
  {
    static CMMC_BME680 *that = this;
    that->interval.every_ms(that->everyMs, []() {
      if (!that->bme->performReading())
      {
        Serial.println("Failed to perform reading :(");
        return;
      }
      that->data.field1 = that->bme->temperature * 100;
      that->data.field2 = that->bme->humidity * 100;
      that->data.field3 = that->bme->pressure;
      that->data.field4 = that->bme->gas_resistance;
      that->cb((void *)&that->data, sizeof(that->data));
    });
  };
};
