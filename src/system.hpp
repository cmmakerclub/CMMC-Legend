#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <SPIFFSEditor.h>

#include <CMMC_Blink.hpp>
#include <CMMC_Interval.hpp>
#include <CMMC_Config_Manager.h>

MqttConnector *mqtt;
uint32_t lastRecv;

CMMC_Interval interval;
CMMC_Blink *blinker;

enum MODE {SETUP, RUN};
MODE mode;

CMMC_Config_Manager mqttConfigManager;
CMMC_Config_Manager wifiConfigManager;
CMMC_Config_Manager sensorsConfigManager;
CMMC_Config_Manager bmeConfigManager;

char sta_ssid[30] = "";
char sta_pwd[30] = "";

char ap_ssid[30] = "CMMC-Legend";
char ap_pwd[30] = "";

char mqtt_host[40] = "";
char mqtt_user[40] = "";
char mqtt_pass[40] = "";
char mqtt_clientId[40] = "";
char mqtt_prefix[40] = "";
char mqtt_port[10] = "";
char mqtt_device_name[15] = "";

int bmeType;
int bmeEnable;

int dhtType;
int dhtPin;
int dhtEnable;


void checkConfigMode();

void init_gpio() {
  SPIFFS.begin();
  blinker = new CMMC_Blink;
  blinker->init();
  blinker->setPin(2);
  pinMode(0, INPUT_PULLUP);
  Serial.begin(57600);
  Serial.println();
  Serial.println();
  Serial.println();
  blinker->blink(500);
  delay(10);
}
void init_sta() {
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  delay(20);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(ap_ssid);
  WiFi.begin(sta_ssid, sta_pwd);
  digitalWrite(LED_BUILTIN, HIGH);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", sta_ssid, sta_pwd);
    checkConfigMode();
    delay(300);
  }
}

void init_ap() {
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  delay(20);
}

void init_userconfig() {
  wifiConfigManager.init("/wifi.json");
  mqttConfigManager.init("/mymqtt.json");
  sensorsConfigManager.init("/sensors.json");

  wifiConfigManager.load_config([](JsonObject * root, const char* content) {
    if (root == NULL) {
      Serial.println("load wifi failed.");
      Serial.print(">");
      Serial.println(content);
      return ;
    }
    Serial.println("[user] wifi config json loaded..");
    Serial.print(">");
    Serial.println(content);
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
    Serial.println("[user] mqtt config json loaded.. >");
    Serial.print(">");
    Serial.println(content);
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
      strcpy(mqtt_host, mqtt_configs[0]);
      strcpy(mqtt_user, mqtt_configs[1]);
      strcpy(mqtt_pass, mqtt_configs[2]);
      strcpy(mqtt_clientId, mqtt_configs[3]);
      strcpy(mqtt_port, mqtt_configs[4]);
      strcpy(mqtt_device_name, mqtt_configs[5]);
      strcpy(mqtt_prefix, mqtt_configs[6]);

      bool lwt = String(mqtt_configs[7]).toInt();
      uint32_t port = String(mqtt_configs[4]).toInt();
      uint32_t pubEveryS = String(mqtt_configs[8]).toInt();

      if (strcmp(mqtt_device_name, "") == 0) {
        sprintf(mqtt_device_name, "%08x", ESP.getChipId());
      }

      if (strcmp(mqtt_clientId, "") == 0) {
        sprintf(mqtt_clientId, "%08x", ESP.getChipId());
      }

      MQTT_HOST = String(mqtt_host);
      MQTT_USERNAME = String(mqtt_user);
      MQTT_PASSWORD = String(mqtt_pass);
      MQTT_CLIENT_ID = String(mqtt_clientId);
      MQTT_PORT = String(mqtt_port).toInt();
      MQTT_PREFIX = String(mqtt_prefix);
      PUBLISH_EVERY = pubEveryS * 1000L;
      MQTT_LWT = lwt;
      DEVICE_NAME = String(mqtt_device_name);
    }
  });

  sensorsConfigManager.load_config([](JsonObject * root, const char* content) {
    Serial.println("loading dht..");
    if (root == NULL) {
      Serial.println("load dht failed.");
      Serial.print(">");
      Serial.println(content);
      return ;
    }
    Serial.print(">");
    Serial.println(content);
    const char* dht_configs[] = { (*root)["dht_pin"], (*root)["dht_type"], (*root)["dht_enable"] };
    dhtPin = String(dht_configs[0]).toInt();
    dhtType = String(dht_configs[1]).toInt();
    dhtEnable = String(dht_configs[2]).toInt();
  });

  bmeConfigManager.load_config([](JsonObject * root, const char* content) {
    Serial.println("loading bme..");
    if (root == NULL) {
      Serial.println("load bmefailed.");
      Serial.print(">");
      Serial.println(content);
      return ;
    }
    Serial.print(">");
    Serial.println(content);
    const char* bme_configs[] = { (*root)["bme_pin"], (*root)["bme_type"], (*root)["bme_enable"] };
    bmeType = String(bme_configs[1]).toInt();
    bmeEnable = String(bme_configs[2]).toInt();

    if (bmeEnable) {
      Serial.printf("BME SENSOR ENABLED: %d\r\n", bmeType);
      if (bmeType == 280) {
        Serial.println("FOUND BME 280");
      }
      else if (bmeType == 680) {
        Serial.println("FOUND BME 680");
      }
      else {
        Serial.println("INVALID BME TYPE");
      }
    }
    else {
      Serial.println("BME SENSOR DISABLED.");
    }
  });
}

void checkConfigMode() {
  uint32_t prev = millis();
  while (digitalRead(0) == LOW) {
    delay(50);
    if (millis() - prev > 2000L) {
      Serial.println("LONG PRESSED.");
      blinker->blink(50);
      while (digitalRead(0) == LOW) {
        delay(10);
      }
      SPIFFS.remove("/enabled");
      delay(300);
      ESP.restart();
    }
  }
}

void select_bootmode() {
  if (!SPIFFS.exists("/enabled")) {
    blinker->blink(50);
    Serial.println("AP Only Mode.");
    mode = SETUP;
    Serial.printf("ESP8266 Chip id = %08X\n", ESP.getChipId());
    sprintf(&ap_ssid[5], "%08x", ESP.getChipId());
    init_ap();
    WiFi.softAP(ap_ssid, &ap_ssid[5]);
    setupWebServer();
  }
  else {
    mode = RUN;
    init_sta();
    Serial.println("WiFi Connected.");
    blinker->blink(4000);
    lastRecv = millis();
    init_mqtt();
  }
}


void run() {
  if (mode == RUN) {
    interval.every_ms(10L * 1000, []() {
      Serial.printf("Last Recv %lu ms ago.\r\n", (millis() - lastRecv));
      if ( (millis() - lastRecv) > (PUBLISH_EVERY * 3) ) {
        ESP.restart();
      }
    });
    mqtt->loop();
  }
  checkConfigMode();
}