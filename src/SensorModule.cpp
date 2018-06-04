#include "SensorModule.h"

#define SENSOR_CONFIG_FILE "/sensors.json"
static CMMC_ConfigManager *m2 = new CMMC_ConfigManager(SENSOR_CONFIG_FILE);

void SensorModule::config(CMMC_System *os, AsyncWebServer* server) {
  strcpy(this->path, "/api/sensors");
  strcpy(this->config_file, SENSOR_CONFIG_FILE);
  static SensorModule *that = this;
  this->_serverPtr = server;
  this->_managerPtr = m2;
  this->_managerPtr->init();
  this->_managerPtr->load_config([&](JsonObject * root, const char* content) {
    if (root == NULL) {
      Serial.print("sensor.json failed. >");
      Serial.println(content);
      return ;
    }
    Serial.println("[user] sensor config json loaded..");
    const char* config[2];
    config[0] = (*root)["sensorType"];
    // config[1] = (*root)["sensorId"];
    strcpy(sensorName, String(config[0]).c_str());
  });
  this->configWebServer();
}

void SensorModule::configWebServer() {
  static SensorModule *that = this;
  strcpy(that->_managerPtr->filename_c, config_file);; 
  _serverPtr->on(this->path, HTTP_POST, [](AsyncWebServerRequest * request) {
    String output = that->saveConfig(request, m2);
    request->send(200, "application/json", output);
  }); 
}
void SensorModule::setup() {
}

void SensorModule::loop() { }
