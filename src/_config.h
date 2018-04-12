#include <Arduino.h>

/* BOARD INFO */
String DEVICE_NAME      = "CMMC-LEGEND001"; 

/* WIFI INFO */ 
String WIFI_SSID        = "CMMC-3rd";
String WIFI_PASSWORD    = "espertap";

/* MQTT INFO */ 
String MQTT_HOST        = "odin.cmmc.io";
String MQTT_USERNAME    = "cmmc";
String MQTT_PASSWORD    = "cmmc";
String MQTT_CLIENT_ID   = "";
String MQTT_PREFIX      = "CMMC/";
int    MQTT_PORT        = 1883;
int PUBLISH_EVERY       = 10000;
int MQTT_CONNECT_TIMEOUT= 5000; 

