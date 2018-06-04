#define CMMC_USE_ALIAS 
#include <Arduino.h>

#include "version.h"
#include "CMMC_Legend.h"
#include "ESPNowModule.h" 
#include "WiFiModule.h" 
#include "MqttModule.h" 
#include "SensorModule.h" 

CMMC_Legend os;

void setup()
{
  os.addModule(new SensorModule());
  // os.addModule(new WiFiModule());
  // os.addModule(new MqttModule);;
  // os.addModule(new ESPNowModule());
  os.setup();
  Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop()
{
  os.run();
}