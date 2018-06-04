#include <CMMC_Module.h> 
#ifndef CMMC_MQTT_MODULE_H
#define CMMC_MQTT_MODULE_H 

#include <MqttConnector.h>

class MqttModule: public CMMC_Module {
  public:
    void config(CMMC_System *os, AsyncWebServer* server); 
    void setup();
    void loop(); 
  protected:
    void configWebServer();
    MqttConnector* init_mqtt();
  private: 
    MqttConnector* mqtt; 
    String MQTT_HOST;
    String MQTT_USERNAME;
    String MQTT_PASSWORD;
    String MQTT_CLIENT_ID;
    String MQTT_PREFIX;
    String DEVICE_NAME;
    int MQTT_PORT;
    int PUBLISH_EVERY;
    int MQTT_CONNECT_TIMEOUT;

    bool MQTT_LWT;
    char myName[15]; 
};

#endif