#include "CMMC_Module.h"

String CMMC_Module::saveConfig(AsyncWebServerRequest *request, CMMC_ConfigManager* configManager, const char* config_file) {
  strcpy(this->config_file, config_file);
  int params = request->params();
  String output = "{";
  for (int i = 0; i < params; i++) {
    AsyncWebParameter* p = request->getParam(i);
    if (p->isPost()) {
      const char* key = p->name().c_str();
      const char* value = p->value().c_str();
      String v;
      if (value == 0) {
        Serial.println("value is null..");
        v = String("");
      }
      else {
        v = String(value);
      }
      output += "\"" + String(key) + "\"";
      if (i == params - 1 ) {
        output += ":\"" + v + "\"";
      }
      else {
        output += ":\"" + v + "\",";
      }
      configManager->add_field(key, v.c_str());
    }
  }
  output += "}";
  configManager->commit(config_file);
  return output;
} 


void CMMC_Module::configLoop() {
  yield(); 
}