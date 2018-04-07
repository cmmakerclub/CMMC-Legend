#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <CMMC_Blink.hpp>


CMMC_Blink *blinker;
const char* http_username = "admin";
const char* http_password = "admin";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

#include "webserver.h"

const char* hostName = "CMMC-Legend";

void setup() {
    blinker = new CMMC_Blink;
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);
    delay(1000);
    blinker->init();
    blinker->blink(500, 2);
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

