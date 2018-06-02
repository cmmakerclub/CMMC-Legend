#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h> 
#include "version.h"
#include "system.hpp"

// MQTT CONNECTOR
char myName[40];

bool flag_restart = false; 
CMMC_Legend os;




void setup()
{
  // os.addPlugin()
  os.setup(); 
  Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop() 
{
  os.run();
}