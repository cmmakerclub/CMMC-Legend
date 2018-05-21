#include <AsyncWebSocket.h>
extern CMMC_Config_Manager wifiConfigManager;
extern CMMC_Config_Manager mqttConfigManager;

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern AsyncEventSource events;
extern CMMC_Interval interval;
extern const char* http_username;
extern const char* http_password;
extern bool flag_restart;

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
void setupWebServer() {
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  events.onConnect([](AsyncEventSourceClient * client) {
    client->send("hello!", NULL, millis(), 1000);
  });
  server.addHandler(&events);
  server.addHandler(new SPIFFSEditor(http_username, http_password));

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server.on("/enable", HTTP_GET, [](AsyncWebServerRequest * request) {
    File f = SPIFFS.open("/enabled", "a+");
    if (!f) {
        Serial.println("file open failed");
    }
    request->send(200, "text/plain", String("ENABLING.. ") + String(ESP.getFreeHeap()));
  });


  static const char* fsServerIndex = "<form method='POST' action='/do-fs' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>"; 
  static const char* serverIndex = "<form method='POST' action='/do-' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>"; 
  server.on("/firmware", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", serverIndex);
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/fs", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", fsServerIndex);
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/do-fs", HTTP_POST, [](AsyncWebServerRequest * request) {
    // the request handler is triggered after the upload has finished...
    // create the response, add header, and send response
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    // restartRequired = true;  // Tell the main loop to restart the ESP
    request->send(response);
  }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    //Upload handler chunks in data 
    if (!index) { // if index == 0 then this is the first frame of data
      SPIFFS.end();
      Serial.println("upload start...");
      Serial.printf("UploadStart: %s\n", filename.c_str()); 
      // Serial.setDebugOutput(true);

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
      } else {
        Update.printError(Serial);
      }
      // Serial.setDebugOutput(false);
    }
  });

  server.on("/api/wifi/ap", HTTP_POST, [](AsyncWebServerRequest * request) {
    int params = request->params();
        String output = "{";
        for (int i = 0; i < params; i++) {
          AsyncWebParameter* p = request->getParam(i);
          if (p->isPost()) {
            const char* key = p->name().c_str();
            const char* value = p->value().c_str();
            Serial.printf("POST[%s]->%s\n", key, value);
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
            Serial.println("add field.");
            wifiConfigManager.add_field(key, v.c_str());
            Serial.println("/add field.");
          }
        }
    output += "}";
    Serial.println(output);
    request->send(200, "application/json", output);
  });

  // ===== END /API/WIFI/AP ===== 
  // ===== CREATE /API/WIFI/STA ===== 
    server.on("/api/wifi/sta", HTTP_POST, [](AsyncWebServerRequest * request) {
      int params = request->params();
          String output = "{";
          for (int i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);
            if (p->isPost()) {
              const char* key = p->name().c_str();
              const char* value = p->value().c_str();
              Serial.printf("POST[%s]: %s\n", key, value);
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
              wifiConfigManager.add_field(key, v.c_str());
            }
          }
      output += "}";
      Serial.println(output);
      request->send(200, "application/json", output);
      wifiConfigManager.commit();
      flag_restart = 1;
    });
    // ===== END /API/WIFI/STA =====

  server.on("/api/mqtt", HTTP_POST, [](AsyncWebServerRequest * request) {
    int params = request->params();
    String output = "{";
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isPost()) {
        const char* key = p->name().c_str();
        const char* value = p->value().c_str();
        Serial.printf("POST[%s]: %s\n", key, value);
        String v;
        if (value == NULL) {
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
        mqttConfigManager.add_field(key, v.c_str());
      }
    }
    output += "}";
    Serial.println(output);
    request->send(200, "application/json", output);
    mqttConfigManager.commit();
    flag_restart = 1;
  }); 

  server.onNotFound([](AsyncWebServerRequest * request) {
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
    for (i = 0; i < headers; i++) {
      AsyncWebHeader* h = request->getHeader(i);
      // Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

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


  server.begin();
  Serial.println("Starting webserver...");
}
