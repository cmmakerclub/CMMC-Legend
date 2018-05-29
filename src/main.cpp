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

extern int bmeType; 
extern int dhtPin;
extern int dhtType;

CMMC_SENSOR_DATA_T sensorData;

void readSensorCb(void *d, size_t len)
{
  memcpy(&sensorData, d, len); 
  Serial.printf("field1 %lu, field2 %lu \r\n", sensorData.field1, sensorData.field2);
};

void setup()
{
  init_gpio();
  init_userconfig();
  Serial.println("USER MAIN SPACE.");
  Serial.printf("sensor Type = %s \r\n", sensorType);
  String _sensorType = String(sensorType);
  if (_sensorType == "BME280") {
      sensorInstance = new CMMC_BME280;
      sensorInstance->setup();
  }
  else if (_sensorType == "BME680") {
      sensorInstance = new CMMC_BME680;
      sensorInstance->setup();
  }
  else if (_sensorType == "DHT11") {
      sensorInstance = new CMMC_DHT; 
      sensorInstance->setup(dhtPin, 11);
  }
  else if (_sensorType == "DHT22") {
      sensorInstance = new CMMC_DHT; 
      sensorInstance->setup(dhtPin, 22);
  }
  else {
    Serial.println("No sensor selected.");
  }

  if (sensorInstance) {
    sensorInstance->every(10L * 1000);
    sensorInstance->onData(readSensorCb);
    Serial.printf("sensor tag = %s\r\n", sensorInstance->tag.c_str()); 
  }

  select_bootmode();
  // Serial.setDebugOutput(true);
  // WiFi.begin("CMMC-3rd", "espertap");
  Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
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