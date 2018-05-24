#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ArduinoJson.h>
#include <MqttConnector.h>

#include "sensors.hpp"

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

CMMC_Interval sensorInterval;

uint32_t temperature;
uint32_t humidity;
uint32_t gas_resistance;
uint32_t pressure;


CMMC_18B20 dTemp;
CMMC_Gpio gpio;
// CMMC_BME280 bme280;
// CMMC_BME680 bme680;
// CMMC_DHT myDHT;

void readSensorCb(void *d, size_t len)
{
  CMMC_18B20::SENSOR_DATA data;
  // CMMC_BME280::SENSOR_DATA data;
  // CMMC_BME680::SENSOR_DATA data;

  memcpy(&data, d, len);
  Serial.printf("read at %lu\r\n", millis());
  Serial.printf("temp=%lu\r\n", data.temperature);

  // Serial.printf("humid=%lu\r\n", data.humidity);
  // Serial.printf("pressure=%lu\r\n", data.pressure);
  // Serial.printf("alt=%lu\r\n", data.altitude);

  // Serial.printf("gas r=%lu\r\n", data.gas_resistance - gas_resistance);
  // gas_resistance = data.gas_resistance;

  Serial.printf("============\r\n");
};

void setup()
{
  init_gpio();
  init_userconfig();
  select_bootmode();
  gpio.setup();
  dTemp.setup(2);

  dTemp.every(5000);
  dTemp.onData(readSensorCb);

  // bme280.setup();
  // bme280.every(10000);
  // bme280.onData(readSensorCb);
  // bme280.setup();
  // bme280.every(10000);
  // bme280.onData(readSensorCb);
  Serial.setDebugOutput(true);  
  WiFi.begin("CMMC-3rd", "espertap");


  // bme680.setup();
  // bme680.every(10000);
  // bme680.onData(readSensorCb);

  Serial.printf("\r\nAPP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop()
{
  run();
  // bme280.read();
  // bme680.read();
  if (mode == RUN) {
    dTemp.read();
    // bme280.read(); 
  } 
}