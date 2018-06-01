#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

#include <CMMC_Blink.hpp>
#include <CMMC_Interval.hpp>
#include <CMMC_Config_Manager.h>
#include "CMMC_System.hpp"

CMMC_Sensor *sensorInstance; 
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

extern void setupWebServer(AsyncWebServer *, AsyncWebSocket *, AsyncEventSource *); 

struct MQTT_Config_T {
  char mqtt_host[40] = "";
  char mqtt_user[40] = "";
  char mqtt_pass[40] = "";
  char mqtt_clientId[40] = "";
  char mqtt_prefix[40] = "";
  char mqtt_port[10] = "";
  char mqtt_device_name[15] = "";
}; 

enum MODE {SETUP, RUN};

CMMC_Config_Manager mqttConfigManager;
CMMC_Config_Manager wifiConfigManager;
CMMC_Config_Manager sensorsConfigManager;
char sta_ssid[30] = "";
char sta_pwd[30] = "";
char ap_ssid[30] = "CMMC-Legend";
char ap_pwd[30] = "";
MQTT_Config_T mqttConfig;
char sensorType[15];
MqttConnector *mqtt;
uint32_t lastRecv;
CMMC_Interval interval;
CMMC_Blink *blinker;
CMMC_SENSOR_DATA_T sensorData;
void readSensorCb(void *d, size_t len)
{
  memcpy(&sensorData, d, len);
  Serial.printf("field1 %lu, field2 %lu \r\n", sensorData.field1, sensorData.field2);
}; 

std::map<String, CMMC_Config_Manager*> configMap;

class CMMC_Legend: public CMMC_System {
  MODE mode;

  public:
    void setup() {
      CMMC_System::setup();
    }
    void init_fs() {
      SPIFFS.begin();
      Serial.println("--------------------------");
      Dir dir = SPIFFS.openDir("/");
      while (dir.next()) {
        File f = dir.openFile("r");
        // Serial.print(dir.fileName());
        // Serial.println(f.size());
        Serial.printf("> %s \r\n", dir.fileName().c_str());
      }
      Serial.println("--------------------------");
      if (!SPIFFS.exists("/enabled")) {
        mode = SETUP;
      }
      else {
        mode = RUN;
      }
      wifiConfigManager.init("/wifi.json");
      mqttConfigManager.init("/mymqtt.json");
      sensorsConfigManager.init("/sensors.json");
    }

    void init_gpio() {
      pinMode(0, INPUT_PULLUP);
      blinker = new CMMC_Blink;
      blinker->init();
      blinker->setPin(2);
      gpio.setup();
      Serial.begin(57600);
      Serial.println();
      blinker->blink(500);
      delay(10);
    }

    void init_user_sensor() { 
      if (mode == SETUP) return;
      String _sensorType = String(sensorType);
      if (_sensorType == "BME280") {
        sensorInstance = new CMMC_BME280;
        sensorInstance->setup();
      }
      else if (_sensorType == "BME680") {
        sensorInstance = new CMMC_BME680;
        sensorInstance->setup();
      }
      else if (_sensorType == "DHT11") {
        sensorInstance = new CMMC_DHT;
        sensorInstance->setup(12, 11);
      }
      else if (_sensorType == "DHT22") {
        sensorInstance = new CMMC_DHT;
        sensorInstance->setup(12, 22);
      }
      else {
        Serial.println("No sensor selected.");
      }

      if (sensorInstance) {
        sensorInstance->every(10L * 1000);
        sensorInstance->onData(readSensorCb);
        Serial.printf("sensor tag = %s\r\n", sensorInstance->tag.c_str());
      }


    }

    void init_user_config() {
      wifiConfigManager.load_config([](JsonObject * root, const char* content) {
        if (root == NULL) {
          Serial.println("load wifi failed.");
          Serial.print(">");
          Serial.println(content);
          return ;
        }
        Serial.println("[user] wifi config json loaded..");
        const char* sta_config[2];
        sta_config[0] = (*root)["sta_ssid"];
        sta_config[1] = (*root)["sta_password"];
        if ((sta_config[0] == NULL) || (sta_config[1] == NULL)) {
          Serial.println("NULL..");
          SPIFFS.remove("/enabled");
          return;
        };
        strcpy(sta_ssid, sta_config[0]);
        strcpy(sta_pwd, sta_config[1]);
      });

      mqttConfigManager.load_config([](JsonObject * root, const char* content) {
        if (root == NULL) {
          Serial.println("load mqtt failed.");
          Serial.print(">");
          Serial.println(content);
          return ;
        }
        Serial.println("[user] mqtt config json loaded.. ");
        const char* mqtt_configs[] = {(*root)["host"],
                                      (*root)["username"],
                                      (*root)["password"],
                                      (*root)["clientId"],
                                      (*root)["port"],
                                      (*root)["deviceName"],
                                      (*root)["prefix"], // [6]
                                      (*root)["lwt"],
                                      (*root)["publishRateSecond"]
                                     };

        if (mqtt_configs[0] != NULL) {
          strcpy(mqttConfig.mqtt_host, mqtt_configs[0]);
          strcpy(mqttConfig.mqtt_user, mqtt_configs[1]);
          strcpy(mqttConfig.mqtt_pass, mqtt_configs[2]);
          strcpy(mqttConfig.mqtt_clientId, mqtt_configs[3]);
          strcpy(mqttConfig.mqtt_port, mqtt_configs[4]);
          strcpy(mqttConfig.mqtt_device_name, mqtt_configs[5]);
          strcpy(mqttConfig.mqtt_prefix, mqtt_configs[6]);

          bool lwt = String(mqtt_configs[7]).toInt();
          uint32_t port = String(mqtt_configs[4]).toInt();
          uint32_t pubEveryS = String(mqtt_configs[8]).toInt();

          if (strcmp(mqttConfig.mqtt_device_name, "") == 0) {
            sprintf(mqttConfig.mqtt_device_name, "%08x", ESP.getChipId());
          }

          if (strcmp(mqttConfig.mqtt_clientId, "") == 0) {
            sprintf(mqttConfig.mqtt_clientId, "%08x", ESP.getChipId());
          }

          MQTT_HOST = String(mqttConfig.mqtt_host);
          MQTT_USERNAME = String(mqttConfig.mqtt_user);
          MQTT_PASSWORD = String(mqttConfig.mqtt_pass);
          MQTT_CLIENT_ID = String(mqttConfig.mqtt_clientId);
          MQTT_PORT = String(mqttConfig.mqtt_port).toInt();
          MQTT_PREFIX = String(mqttConfig.mqtt_prefix);
          PUBLISH_EVERY = pubEveryS * 1000L;
          MQTT_LWT = lwt;
          DEVICE_NAME = String(mqttConfig.mqtt_device_name);
        }
      });

      sensorsConfigManager.load_config([](JsonObject * root, const char* content) {
        Serial.println("[user] sensors config json loaded..");
        if (root == NULL) {
          Serial.println("load sensors config failed.");
          Serial.print(">");
          Serial.println(content);
          return ;
        }
        const char* sensor_configs[] = {
          (*root)["sensorType"],
          (*root)["dht_pin"],
          (*root)["bme_pin"],
        };

        if (sensor_configs[0] != NULL) {
          strcpy(sensorType, sensor_configs[0]);
          String _s = String(sensorType);
        }
        else if (sensor_configs[1] != NULL) {
          //   dhtPin = String(sensor_configs[1]).toInt();
        }
        else if (sensor_configs[2] != NULL) {
          //   dhtPin = String(sensor_configs[1]).toInt();
        }
      });
    }

    void init_network() {
      if (mode == SETUP) {
        _init_ap();
        setupWebServer(&server, &ws, &events);
        blinker->blink(50);
      }
      else if (mode == RUN) {
        _init_sta();
      }
    }

    void run() {
      if (mode == RUN) {
        static CMMC_Legend *that = this;
        interval.every_ms(10L * 1000, []() {
          Serial.printf("Last Recv %lus ago.\r\n", ((millis() - lastRecv) / 1000));
          if ( (millis() - lastRecv) > (PUBLISH_EVERY * 3) ) {
            ESP.restart();
          }
        });
        mqtt->loop();
      }
      isLongPressed();
    }

    String saveConfig(AsyncWebServerRequest *request, CMMC_Config_Manager* configManager) {
      int params = request->params();
      String output = "{";
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          const char* key = p->name().c_str();
          const char* value = p->value().c_str();
          // Serial.printf("POST[%s]->%s\n", key, value);
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
      configManager->commit();
      return output;
    };


    void isLongPressed() {
      uint32_t prev = millis();
      while (digitalRead(0) == LOW) {
        delay(50);
        if ( (millis() - prev) > 5L * 1000L) {
          Serial.println("LONG PRESSED.");
          blinker->blink(50);
          while (digitalRead(0) == LOW) {
            delay(10);
          }
          SPIFFS.remove("/enabled");
          Serial.println("being restarted.");
          delay(1000);
          ESP.restart();
        }
      }
    }

  private:
    void _init_ap() {
      WiFi.disconnect();
      WiFi.softAPdisconnect();
      delay(10);
      WiFi.mode(WIFI_AP);
      delay(10);
      IPAddress Ip(192, 168, 4, 1);
      IPAddress NMask(255, 255, 255, 0);
      WiFi.softAPConfig(Ip, Ip, NMask);
      sprintf(&ap_ssid[5], "%08x", ESP.getChipId()); 
      WiFi.softAP(ap_ssid, &ap_ssid[5]); 
      delay(20);
      IPAddress myIP = WiFi.softAPIP();
      Serial.println();
      Serial.print("AP IP address: ");
      Serial.println(myIP);
    }

    void select_bootmode() {
      init_mqtt();
      lastRecv = millis();
      blinker->blink(4000);
    }

    void _init_sta() {
      WiFi.softAPdisconnect();
      WiFi.disconnect();
      delay(20);
      WiFi.mode(WIFI_STA);
      WiFi.hostname(ap_ssid);
      WiFi.begin(sta_ssid, sta_pwd);
      digitalWrite(LED_BUILTIN, HIGH);
      while (WiFi.status() != WL_CONNECTED) {
        Serial.printf ("Connecting to %s:%s\r\n", sta_ssid, sta_pwd);
        isLongPressed();
        delay(300);
      }
    }
};
