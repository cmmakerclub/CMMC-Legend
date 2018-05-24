#include <CMMC_Sensor.hpp>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Adafruit_BME680.h"
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>

#define SEALEVELPRESSURE_HPA (1013.25)

class CMMC_DHT : public CMMC_Sensor
{
public:
  typedef struct SENSOR_DATA
  {
    uint32_t temperature;
    uint32_t humidity;
  };

  SENSOR_DATA data;
  DHT *dht;
  void setup(int pin, int type)
  {
    dht = new DHT(pin, type);
    dht->begin();
    data.temperature = dht->readTemperature() * 100;
    data.humidity = dht->readHumidity() * 100;
  };

  void read()
  {
    static CMMC_DHT *that = this;
    that->interval.every_ms(that->everyMs, []() {
      that->data.temperature = that->dht->readTemperature() * 100;
      that->data.humidity = that->dht->readHumidity() * 100;
      that->cb((void *)&that->data, sizeof(that->data));
    });
  }
};

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

  void setup()
  {
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
      that->data.pressure = that->bme->readPressure() / 100.0;
      that->data.altitude = that->bme->readAltitude(SEALEVELPRESSURE_HPA);
      that->cb((void *)&that->data, sizeof(that->data));
    });
  };
};

class CMMC_BME680 : public CMMC_Sensor
{
private:
  Adafruit_BME680 *bme; // I2C
public:
  typedef struct SENSOR_DATA
  {
    uint32_t temperature;
    uint32_t humidity;
    uint32_t gas_resistance;
    uint32_t pressure;
  };

  SENSOR_DATA data;

  void setup()
  {
    bme = new Adafruit_BME680;
    if (!bme->begin())
    {
      Serial.println("Could not find a valid BME680 sensor, check wiring!");
    }
    else
    {
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
      that->data.temperature = that->bme->temperature * 100;
      that->data.humidity = that->bme->humidity * 100;
      that->data.pressure = that->bme->pressure;
      that->data.gas_resistance = that->bme->gas_resistance;
      that->cb((void *)&that->data, sizeof(that->data));
    });
  };
};
