#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <CMMC_LED.hpp>
#include <CMMC_Interval.hpp>
#include <CMMC_ConfigManager.h>
#include "CMMC_System.hpp"
#include <vector>
#include "CMMC_Module.h"
#include "version.h"
#include <SPIFFSEditor.h>

#ifndef CMMC_LEGEND_H
#define CMMC_LEGEND_H

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");
static AsyncEventSource events("/events");
static CMMC_LED *blinker;
static const char* http_username = "admin";
static const char* http_password = "admin";

enum MODE {SETUP, RUN};

class CMMC_Legend: public CMMC_System {
  public:
    void addModule(CMMC_Module* module) {
      _modules.push_back(module);
      Serial.printf("addModule.. size = %d\r\n", _modules.size());
    }

    void run() {
      static CMMC_Legend *that = this;
      int size = _modules.size();
      Serial.printf("module size = %d\r\n", size);
      for (int i = 0 ; i < size; i++) {
        _modules[i]->loop();
      }
      isLongPressed();
      yield();
    }

    void isLongPressed() {
      uint32_t prev = millis();
      while (digitalRead(13) == LOW) {
        delay(50);
        if ( (millis() - prev) > 5L * 1000L) {
          Serial.println("LONG PRESSED.");
          blinker->blink(50);
          while (digitalRead(13) == LOW) {
            delay(10);
          }
          SPIFFS.remove("/enabled");
          Serial.println("being restarted.");
          delay(1000);
          ESP.restart();
        }
      }
    }
    void setup() {
      CMMC_System::setup();
    }

  protected:

    void init_gpio() {
      Serial.begin(57600);
      Serial.println("OS::Init GPIO..");
      pinMode(13, INPUT_PULLUP);
      blinker = new CMMC_LED;
      blinker->init();
      blinker->setPin(2);
      Serial.println();
      blinker->blink(500);
      delay(10);
    }

    void init_fs() {
      Serial.println("OS::Init FS..");
      SPIFFS.begin();
      Dir dir = SPIFFS.openDir("/");
      isLongPressed();
      Serial.println("--------------------------");
      while (dir.next()) {
        File f = dir.openFile("r");
        Serial.printf("> %s \r\n", dir.fileName().c_str());
      }
      /*******************************************
         Boot Mode Selection
       *******************************************/
      Serial.println("--------------------------");
      if (!SPIFFS.exists("/enabled")) {
        mode = SETUP;
      }
      else {
        mode = RUN;
      }
    }

    void init_user_sensor() {
      Serial.printf("Initializing Sensor.. MODE=%s\r\n", mode == SETUP ? "SETUP" : "RUN");
      if (mode == SETUP) {
        return;
      }
    }

    void init_user_config() {
      Serial.println("Initializing ConfigManager files.");
    }

    void init_network() {
      Serial.println("Initializing network.");
      for (int i = 0 ; i < _modules.size(); i++) {
        Serial.printf("call config idx = %d\r\n", i);
        _modules[i]->config(this, &server);
      }
      if (mode == SETUP) {
        _init_ap();
        setupWebServer(&server, &ws, &events);
        blinker->blink(50);
        while (1) {
          for (int i = 0 ; i < _modules.size(); i++) {
            _modules[i]->setup();
          }
          yield();
        }
      }
      else if (mode == RUN) {
        blinker->blink(4000);
        for (int i = 0 ; i < _modules.size(); i++) {
          Serial.printf("call once idx = %d\r\n", i);
          _modules[i]->once();
        }
      }
    }

    CMMC_LED *getBlinker() {
      return blinker;
    }

  private:
    MODE mode;
    std::vector<CMMC_Module*> _modules;
    char ap_ssid[30] = "CMMC-Legend";
    void _init_ap() {
      WiFi.disconnect();
      WiFi.softAPdisconnect();
      delay(10);
      WiFi.mode(WIFI_AP);
      delay(10);
      IPAddress Ip(192, 168, 4, 1);
      IPAddress NMask(255, 255, 255, 0);
      WiFi.softAPConfig(Ip, Ip, NMask);
      sprintf(&this->ap_ssid[5], "%08x", ESP.getChipId());
      WiFi.softAP(ap_ssid, &ap_ssid[5]);
      delay(20);
      IPAddress myIP = WiFi.softAPIP();
      Serial.println();
      Serial.print("AP IP address: ");
      Serial.println(myIP);
    }

    void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
      if (type == WS_EVT_CONNECT) {
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        client->printf("Hello Client %u :)", client->id());
        client->ping();
      } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
      } else if (type == WS_EVT_ERROR) {
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
      } else if (type == WS_EVT_PONG) {
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");
      } else if (type == WS_EVT_DATA) {
        AwsFrameInfo * info = (AwsFrameInfo*)arg;
        String msg = "";
        if (info->final && info->index == 0 && info->len == len) {
          //the whole message is in a single frame and we got all of it's data
          Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
          Serial.printf("[HEX]= %x \r\n", info->len);
          Serial.printf("size = %d \r\n", info->len);
          if (info->opcode == WS_TEXT) {
            for (size_t i = 0; i < info->len; i++) {
              msg += (char) data[i];
            }
          } else {
            char buff[3];
            for (size_t i = 0; i < info->len; i++) {
              sprintf(buff, "%02x ", (uint8_t) data[i]);
              msg += buff ;
            }
          }

          Serial.printf("MESSAGE => %s\n", msg.c_str());
          String header = msg.substring(0, 7);
          String value = msg.substring(7);
          String macStr;
          bool validMessage = 0;
          if (header == "MASTER:" && value.length() == 12) {
            macStr = value;
            validMessage = true;
            // saveConfig(macStr);
          }
          else {
            Serial.print("INVALID:");
            Serial.println(msg);
          }

          if (info->opcode == WS_TEXT)
            if (validMessage) {
              // client->text("I got your text message");
              client->text(macStr);
            }
            else {
              client->text(String("INVALID: ") + msg);
            }
          else
            client->binary("I got your binary message");
        } else {
          //message is comprised of multiple frames or the frame is split into multiple packets
          if (info->index == 0) {
            if (info->num == 0)
              Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
            Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
          }

          Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

          if (info->opcode == WS_TEXT) {
            for (size_t i = 0; i < info->len; i++) {
              msg += (char) data[i];
            }
          } else {
            char buff[3];
            for (size_t i = 0; i < info->len; i++) {
              sprintf(buff, "%02x ", (uint8_t) data[i]);
              msg += buff ;
            }
          }
          Serial.printf("%s\n", msg.c_str());

          if ((info->index + len) == info->len) {
            Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
            if (info->final) {
              Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
              if (info->message_opcode == WS_TEXT)
                client->text("I got your text message");
              else
                client->binary("I got your binary message");
            }
          }
        }
      }
    }

    void setupWebServer(AsyncWebServer *server, AsyncWebSocket *ws, AsyncEventSource *events) {
      // ws->onEvent(this->onWsEvent);
      server->addHandler(ws);
      server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
      events->onConnect([](AsyncEventSourceClient * client) {
        client->send("hello!", NULL, millis(), 1000);
      });
      server->addHandler(events);
      server->addHandler(new SPIFFSEditor(http_username, http_password));

      server->on("/heap", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
      });

      server->on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(200, "text/plain", "OK");
        blinker->blink(20);
        ESP.restart();
      });

      server->on("/enable", HTTP_GET, [](AsyncWebServerRequest * request) {
        File f = SPIFFS.open("/enabled", "a+");
        if (!f) {
          Serial.println("file open failed");
        }
        request->send(200, "text/plain", String("ENABLING.. ") + String(ESP.getFreeHeap()));
        // ESP.restart();
      });

      static const char* fsServerIndex = "<form method='POST' action='/do-fs' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
      static const char* serverIndex = "<form method='POST' action='/do-firmware' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
      server->on("/firmware", HTTP_GET, [](AsyncWebServerRequest * request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html", serverIndex);
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
      });

      server->on("/fs", HTTP_GET, [](AsyncWebServerRequest * request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html", fsServerIndex);
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
      });

      server->on("/do-fs", HTTP_POST, [](AsyncWebServerRequest * request) {
        // the request handler is triggered after the upload has finished...
        // create the response, add header, and send response
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        // restartRequired = true;  // Tell the main loop to restart the ESP
        request->send(response);
      }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        //Upload handler chunks in data
        if (!index) { // if index == 0 then this is the first frame of data
          SPIFFS.end();
          blinker->detach();
          Serial.println("upload start...");
          Serial.printf("UploadStart: %s\n", filename.c_str());
          Serial.setDebugOutput(true);
          // calculate sketch space required for the update
          uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
          bool updateOK = maxSketchSpace < ESP.getFreeSketchSpace();
          if (!Update.begin(maxSketchSpace, U_SPIFFS)) { //start with max available size
            Update.printError(Serial);
          }
          Update.runAsync(true); // tell the updaterClass to run in async mode
        }

        //Write chunked data to the free sketch space
        if (Update.write(data, len) != len) {
          Update.printError(Serial);
        }

        if (final) { // if the final flag is set then this is the last frame of data
          if (Update.end(true)) { //true to set the size to the current progress
            Serial.printf("Update Success: %u B\nRebooting...\n", index + len);
            blinker->blink(1000);
          } else {
            Update.printError(Serial);
          }
          Serial.setDebugOutput(false);
        }
      });

      server->on("/do-firmware", HTTP_POST, [](AsyncWebServerRequest * request) {
        // the request handler is triggered after the upload has finished...
        // create the response, add header, and send response
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        // restartRequired = true;  // Tell the main loop to restart the ESP
        request->send(response);
      }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        //Upload handler chunks in data
        if (!index) { // if index == 0 then this is the first frame of data
          blinker->detach();
          SPIFFS.end();
          Serial.println("upload start...");
          Serial.printf("UploadStart: %s\n", filename.c_str());
          Serial.setDebugOutput(true);
          // calculate sketch space required for the update
          uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
          bool updateOK = maxSketchSpace < ESP.getFreeSketchSpace();
          if (!Update.begin(maxSketchSpace)) { //start with max available size
            Update.printError(Serial);
          }
          Update.runAsync(true); // tell the updaterClass to run in async mode
        }

        //Write chunked data to the free sketch space
        if (Update.write(data, len) != len) {
          Update.printError(Serial);
        }

        if (final) { // if the final flag is set then this is the last frame of data
          if (Update.end(true)) { //true to set the size to the current progress
            Serial.printf("Update Success: %u B\nRebooting...\n", index + len);
            blinker->blink(1000);
          } else {
            Update.printError(Serial);
          }
          Serial.setDebugOutput(false);
        }
      });

      server->onNotFound([](AsyncWebServerRequest * request) {
        Serial.printf("NOT_FOUND: ");
        if (request->method() == HTTP_GET)
          Serial.printf("GET");
        else if (request->method() == HTTP_POST)
          Serial.printf("POST");
        else if (request->method() == HTTP_DELETE)
          Serial.printf("DELETE");
        else if (request->method() == HTTP_PUT)
          Serial.printf("PUT");
        else if (request->method() == HTTP_PATCH)
          Serial.printf("PATCH");
        else if (request->method() == HTTP_HEAD)
          Serial.printf("HEAD");
        else if (request->method() == HTTP_OPTIONS)
          Serial.printf("OPTIONS");
        else
          Serial.printf("UNKNOWN");
        Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

        if (request->contentLength()) {
          Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
          Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
        }

        int headers = request->headers();
        int i;
        // for (i = 0; i < headers; i++) {
        //   // AsyncWebHeader* h = request->getHeader(i);
        //   // Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
        // }

        int params = request->params();
        for (i = 0; i < params; i++) {
          AsyncWebParameter* p = request->getParam(i);
          if (p->isFile()) {
            Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
          } else if (p->isPost()) {
            Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
          } else {
            Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
          }
        }

        request->send(404);
      });


      server->begin();
      Serial.println("Starting webserver->..");
    }

};

#endif