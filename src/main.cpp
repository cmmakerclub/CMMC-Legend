#include <Arduino.h>
#include <CMMC_18B20.hpp>
#include <CMMC_DHT.hpp>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ArduinoJson.h>
#include <MqttConnector.h>

#include <CMMC_BME680.hpp>
#include <CMMC_BME280.hpp>
#include <CMMC_18B20.hpp>
#include <CMMC_DHT.hpp>
#include <CMMC_HX711.hpp>
#include <CMMC_VL53L0X.hpp>
#include "gpio.hpp"

#include "version.h"
#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"
#include "_config.h"

#include "system.hpp"
#include "webserver.h"

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


CMMC_Legend os;

void setup()
{
  Serial.println("USER MAIN SPACE.");
  Serial.printf("sensor Type = %s \r\n", sensorType);
  os.setup(); 
  sensorInstance = new CMMC_VL53L0X;
  sensorInstance->setup();

  if (sensorInstance) {
    sensorInstance->every(10L * 1000);
    sensorInstance->onData(readSensorCb);
    Serial.printf("sensor tag = %s\r\n", sensorInstance->tag.c_str());
  } 
  Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
  os.setup(); 
  // sensorInstance = new CMMC_HX711;
  // sensorInstance->setup(12, 14); 
  // Serial.setDebugOutput(true);
  // WiFi.begin("CMMC-3rd", "espertap");
}

void loop() 
{
  os.run();
}