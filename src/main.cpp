#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>


const char* http_username = "admin";
const char* http_password = "admin";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

#include "webserver.h"

const char* hostName = "CMMC-Legend";

void setup() {
    Serial.begin(57600);
    Serial.setDebugOutput(true);
    WiFi.hostname(hostName);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(hostName);
    WiFi.begin("Nat", "espertap");    
    SPIFFS.begin();
    setupWebServer();
}

void loop() {
    // put your main code here, to run repeatedly:
}

