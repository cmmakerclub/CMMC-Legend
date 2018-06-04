#include "MqttModule.h"


#define MQTT_CONFIG_FILE "/mymqtt.json"
static CMMC_ConfigManager *mmm = new CMMC_ConfigManager(MQTT_CONFIG_FILE);

void MqttModule::config(CMMC_System *os, AsyncWebServer* server) {
  strcpy(this->path, "/api/mqtt");
  strcpy(this->config_file, MQTT_CONFIG_FILE);
  this->_serverPtr = server;
  this->_managerPtr = mmm;
  this->_managerPtr->init();
  this->_managerPtr->load_config([](JsonObject * root, const char* content) {
    if (root == NULL) {
      Serial.print("mqtt.json failed. >");
      Serial.println(content);
      return ;
    }
    Serial.println("[user] mqtt config json loaded..");
  });
    this->configWebServer();
}; 

void MqttModule::configWebServer() {
  static MqttModule *that = this;
  Serial.printf("configManager addr %x \r\n", mmm);
  strcpy(that->_managerPtr->filename_c, config_file);; 
  _serverPtr->on(this->path, HTTP_POST, [](AsyncWebServerRequest * request) {
    String output = that->saveConfig(request, mmm);
    request->send(200, "application/json", output);
  }); 
}

void MqttModule::setup() {
  Serial.println("MqttModule::setup"); 
};

void MqttModule::loop() {
  yield();
};  
