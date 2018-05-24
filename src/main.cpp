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
// CMMC_DHT myDHT;

void readSensorCb(void *d, size_t len) {
    CMMC_BME680::SENSOR_DATA data;
    memcpy(&data, d, len);
    Serial.printf("read at %lu\r\n", millis());
    Serial.printf("temp=%lu\r\n", data.temperature);
    Serial.printf("humid=%lu\r\n", data.humidity);
    Serial.printf("pressure=%lu\r\n", data.pressure);
    Serial.printf("gas r=%lu\r\n", data.gas_resistance-gas_resistance);
    Serial.printf("============\r\n"); 
    gas_resistance = data.gas_resistance;
};

void setup() {
  init_gpio();
  init_userconfig();
  select_bootmode(); 
  bme680.setup(); 
  bme680.every(10000);
  bme680.onData(readSensorCb);

  Serial.printf("\r\nAPP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop() {
  run(); 
  bme680.read();
}