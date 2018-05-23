#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ArduinoJson.h>
#include <MqttConnector.h>

#include "version.h"
#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"
#include "_config.h"

#include "webserver.h"
#include "system.hpp"

// MQTT CONNECTOR
char myName[40];

bool flag_mqtt_available = false;
bool flag_restart = false;
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define SEALEVELPRESSURE_HPA (1013.25)
CMMC_Interval sensorInterval;

uint32_t temperature;
uint32_t humidity;
uint32_t gas_resistance;
uint32_t pressure;
#include <CMMC_Sensor.hpp>

class CMMC_BME: public CMMC_Sensor {
  private:
    Adafruit_BME680 bme; // I2C
  public:
    typedef struct SENSOR_DATA {
      uint32_t temperature;
      uint32_t humidity;
      uint32_t gas_resistance;
      uint32_t pressure;
    };
    void setup() {
      if (!bme.begin()) {
        Serial.println("Could not find a valid BME680 sensor, check wiring!");
      }
      else {
        // Set up oversampling and filter initialization
        Serial.println("setup ok");
        bme.setTemperatureOversampling(BME680_OS_8X);
        bme.setHumidityOversampling(BME680_OS_2X);
        bme.setPressureOversampling(BME680_OS_4X);
        bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme.setGasHeater(320, 150); // 320*C for 150 ms
      }
    };

    int read(uint32_t every, callback_t cb) {
      static callback_t c = cb;
      static CMMC_BME* that = this;
      static SENSOR_DATA data;
      that->interval.every_ms(every, []() {
        if (!that->bme.performReading()) {
          Serial.println("Failed to perform reading :(");
          return;
        }

        data.temperature = that->bme.temperature * 100;
        data.humidity = that->bme.humidity * 100;
        data.pressure = that->bme.pressure;
        data.gas_resistance = that->bme.gas_resistance;
        Serial.print("Temperature = ");
        Serial.print(that->bme.temperature);
        Serial.println(" *C");

        Serial.print("Pressure = ");
        Serial.print(that->bme.pressure / 100.0);
        Serial.println(" hPa");

        Serial.print("Humidity = ");
        Serial.print(that->bme.humidity);
        Serial.println(" %");

        Serial.print("Gas = ");
        Serial.print(that->bme.gas_resistance / 1000.0);
        Serial.println(" KOhms");
        Serial.println();
        c( (void*) &data);
      });
    };
};

CMMC_BME hello;
void setup() {
  init_gpio();
  init_userconfig();
  select_bootmode();
  hello.setup();
  Serial.printf("\r\nAPP VERSION: %s\r\n", LEGEND_APP_VERSION);

}

void loop() {
  run();
  hello.read(6000, [](void *d) {
    CMMC_BME::SENSOR_DATA data;
    memcpy(&data, d, sizeof (CMMC_BME::SENSOR_DATA));
    Serial.println(data.temperature);
    Serial.println(data.humidity);
    Serial.println(data.pressure);
  });
}