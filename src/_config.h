#include <Arduino.h>



/* BOARD INFO */
String DEVICE_NAME      = "CMMC-LEGEND001"; 


/* MQTT INFO */ 
String MQTT_HOST;
String MQTT_USERNAME;
String MQTT_PASSWORD;
String MQTT_CLIENT_ID;
String MQTT_PREFIX;
int    MQTT_PORT;
int PUBLISH_EVERY       = 5000;
int MQTT_CONNECT_TIMEOUT= 5000; 

