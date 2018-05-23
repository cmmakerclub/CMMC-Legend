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

CMMC_Interval sensorInterval;

uint32_t temperature;
uint32_t humidity;
uint32_t gas_resistance;
uint32_t pressure;

#include "sensors.hpp" 

CMMC_BME680 bme680;
CMMC_DHT myDHT;

void setup() {
  init_gpio();
  init_userconfig();
  select_bootmode();

  myDHT.setup(12, 22);

  Serial.printf("\r\nAPP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop() {
  run();

  myDHT.read(6000, [](void *d, size_t len) {
    CMMC_DHT::SENSOR_DATA data;
    memcpy(&data, d, len);
    Serial.println(data.temperature);
    Serial.println(data.humidity);
  });
}