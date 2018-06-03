#define CMMC_USE_ALIAS 
#include <Arduino.h>
#include "version.h"
#include "CMMC_Legend.hpp"
#include "ESPNowModule.hpp"

CMMC_Legend os;

void setup()
{
  // os.addModule(new WiFiModule());
  os.addModule(new ESPNowModule());
  os.setup();
  Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop()
{
  os.run();
}