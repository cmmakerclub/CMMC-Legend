#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "version.h"
#include "system.hpp"
#include "CMMC_Module.hpp"

bool flag_restart = false;
CMMC_Legend os;

void setup()
{
  // os.addModule(new WiFiModule());
  os.setup();
  Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop()
{
  os.run();
}