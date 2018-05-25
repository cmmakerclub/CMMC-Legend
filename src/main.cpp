#include <Arduino.h>

#include <CMMC_18B20.hpp> 
#include <CMMC_DHT.hpp>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ArduinoJson.h>
#include <MqttConnector.h>

#include <CMMC_BME680.hpp>
#include <CMMC_BME280.hpp>
#include "gpio.hpp"

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

CMMC_Sensor *sensorInstance;

CMMC_18B20 dTemp;
CMMC_Gpio gpio;

extern int bmeEnable;
extern int bmeType;

extern int dhtEnable;
extern int dhtPin;
extern int dhtType;

void readSensorCb(void *d, size_t len)
{
  CMMC_BME680::SENSOR_DATA data; 
  memcpy(&data, d, len);
  Serial.printf("read at %lu\r\n", millis());
  Serial.printf("temp=%lu\r\n", data.temperature);
  Serial.printf("humid=%lu\r\n", data.humidity); 
  Serial.printf("============\r\n");
};

void setup()
{
  init_gpio();
  gpio.setup();
  init_userconfig();
  if (bmeEnable) {
    Serial.printf("bme enabled type = %d\r\n", bmeType);
    if (bmeType == 280) {
      sensorInstance = new CMMC_BME280;
    }
    else {
      sensorInstance = new CMMC_BME680;
    }
    sensorInstance->setup();
  }
  else if (dhtEnable) {
    Serial.println("DHT ENABLED.");
    sensorInstance = new CMMC_DHT; 
    sensorInstance->setup(dhtPin, dhtType);
  }
  if (sensorInstance) {
    sensorInstance->every(10L * 1000);
    sensorInstance->onData(readSensorCb);
    Serial.printf("sensor tag = %s\r\n", sensorInstance->tag.c_str()); 
  }
  select_bootmode();
  Serial.setDebugOutput(true);
  // WiFi.begin("ampere", "espertap");
  Serial.printf("\r\nAPP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop()
{
  run();
  if (mode == RUN) {
    if (sensorInstance) {
      sensorInstance->read();
    }
  }
}