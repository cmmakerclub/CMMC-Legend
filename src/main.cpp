#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <CMMC_Blink.hpp>
#include <CMMC_Interval.hpp>
#include "ESP8266WiFi.h"

CMMC_Blink *blinker;
const char* http_username = "admin";
const char* http_password = "admin";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
CMMC_Interval interval;

#include "webserver.h"

const char* hostName = "CMMC-Legend";
String output;

void scanAndUpdateSSIDoutput();
void setup() {
    blinker = new CMMC_Blink;
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    blinker->init();
    blinker->blink(500, 2);
    Serial.begin(57600);
    Serial.setDebugOutput(true);
    WiFi.softAPdisconnect();
    WiFi.disconnect();
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    delay(20);
    WiFi.mode(WIFI_STA);
    scanAndUpdateSSIDoutput();
    WiFi.hostname(hostName);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(hostName);
    WiFi.begin("Nat", "espertap");
    SPIFFS.begin();
    setupWebServer();
    
}

void scanAndUpdateSSIDoutput() {
    int n = WiFi.scanNetworks();
    Serial.println(n);
    String currentSSID = WiFi.SSID();
    output = "[";
    for( int i=0; i<n; i++ ) {
    if (output != "[") output += ',';
        output += "{\"name\": ";
        output += "\"";
        output += WiFi.SSID(i);
        output += "\"";
        output += "}";
        Serial.println(WiFi.SSID(i));
        yield();
    }
    output += "]";
    Serial.println(output);
}

void loop() {
    interval.every_ms(10L*1000, []() {
        scanAndUpdateSSIDoutput();
    });
    // put your main code here, to run repeatedly:
}

