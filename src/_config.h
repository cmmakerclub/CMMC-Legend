#include <Arduino.h>

/* BOARD INFO */
String DEVICE_NAME; 

/* MQTT INFO */ 
String MQTT_HOST;
String MQTT_USERNAME;
String MQTT_PASSWORD;
String MQTT_CLIENT_ID;
String MQTT_PREFIX;
int    MQTT_PORT;
int PUBLISH_EVERY;
int MQTT_CONNECT_TIMEOUT= 500;
bool MQTT_LWT = false;

