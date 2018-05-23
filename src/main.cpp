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

Adafruit_BME680 bme; // I2C

uint32_t temperature;
uint32_t humidity;
uint32_t gas_resistance;
uint32_t pressure;

void setup() {
  init_gpio(); 
  init_userconfig(); 
  select_bootmode();
  Serial.printf("\r\nAPP VERSION: %s\r\n", LEGEND_APP_VERSION); 

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
  } 
  else {
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms 
  }
} 

void loop() { 
  run();
  sensorInterval.every_ms(5L*1000, []() {
    if (mode != RUN) return;
    if (!bme.performReading()) {
      Serial.println("Failed to perform reading :(");
      return;
    }

    temperature = bme.temperature*100;
    humidity = bme.humidity*100;
    pressure = bme.pressure;
    gas_resistance = bme.gas_resistance;
    Serial.print("Temperature = ");
    Serial.print(bme.temperature);
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bme.pressure / 100.0);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(bme.humidity);
    Serial.println(" %");

    Serial.print("Gas = ");
    Serial.print(bme.gas_resistance / 1000.0);
    Serial.println(" KOhms");
    Serial.println();

  });
} 