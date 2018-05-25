#include <Arduino.h>
#include <CMMC_Sensor.hpp>

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
    Serial.printf("DHT BEGIN... pin=%d, type=%d\r\n", pin, type);
    this->tag = String("DHT")+type;
    dht = new DHT(pin, type);
    dht->begin();
    data.temperature = dht->readTemperature() * 100;
    data.humidity = dht->readHumidity() * 100;
    Serial.printf("temp %lu, hudmid %lu \r\n", data.temperature, data.humidity);
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
