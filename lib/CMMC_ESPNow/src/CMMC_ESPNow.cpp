#define CMMC_USE_ALIAS

#include "CMMC_ESPNow.h"
#include <CMMC_Utils.h>

typedef struct __attribute((__packed__)) {
  uint8_t from[6];
  uint8_t to[6];
  uint8_t type = 0;
  uint16_t battery = 0x00;
  uint32_t field1 = 0x00;
  uint32_t field2 = 0x00;
  uint32_t field3 = 0x00;
  uint32_t field4 = 0x00;
  uint32_t field5 = 0x00;
  uint32_t field6 = 0x00;
  uint8_t nameLen = 0x00;
  char myName[16];
  uint32_t ms = 0;
  uint16_t sent_ms = 0;
  uint32_t sum = 0;
} CMMC_SENSOR_T;

extern uint8_t Send_complete;

CMMC_ESPNow::CMMC_ESPNow() {
  static CMMC_ESPNow* that = this;
  this->_user_debug_cb = [](const char* s) { };
  this->_user_on_message_recv = [](uint8_t *macaddr, uint8_t *data, uint8_t len) {};
  this->_user_on_message_sent = [](uint8_t *macaddr, u8 status) {};

  // set system cb
  this->_system_on_message_recv = [](uint8_t *macaddr, uint8_t *data, uint8_t len) {
    that->_user_debug_cb("_system_on_message_recv");
    that->_waiting_message_has_arrived = true;
    that->_user_on_message_recv(macaddr, data, len);
  };

  this->_system_on_message_sent = [](uint8_t *macaddr, u8 status) {
    that->_message_sent_status = status;
    that->_user_on_message_sent(macaddr, status);
  };
}

void CMMC_ESPNow::init(int mode) {
  WiFi.disconnect();
  if (mode == NOW_MODE_SLAVE) {
    WiFi.mode(WIFI_STA);
  }
  else {
    WiFi.mode(WIFI_STA);
  }

  if (esp_now_init() == 0) {
		USER_DEBUG_PRINTF("espnow init ok");
  } else {
		USER_DEBUG_PRINTF("espnow init failed");
    ESP.restart();
    return;
  }

  if (mode == NOW_MODE_CONTROLLER) {
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  } else {
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  }

  esp_now_register_send_cb(this->_system_on_message_sent);
  esp_now_register_recv_cb(this->_system_on_message_recv);
};


void CMMC_ESPNow::send(uint8_t *mac, u8* data, int len, void_cb_t cb, uint32_t wait_time) {
  if (mac[0] == 0x00 && mac[1] == 0x00) {
    return;
  }
  this->_message_sent_status = -1;
  this->_waiting_message_has_arrived = false;
  CMMC_SENSOR_T *packet;
  packet = (CMMC_SENSOR_T *)data;
  uint32_t MAX_RETRIES   = 15;
  uint32_t RETRIES_DELAY = 20;
  int retries = 0;

  esp_now_send(mac, data, len);
  delay(RETRIES_DELAY*(retries+1));

  if (this->_enable_retries) {
    while(this->_message_sent_status != 0) {
      USER_DEBUG_PRINTF("try to send over espnow...");

      esp_now_send(mac, data, len);
      delay(RETRIES_DELAY*(retries+1));
      Serial.println("retries");
      if (++retries > MAX_RETRIES) {
        break;
      }
      packet->field6= retries;
      packet->sum= CMMC::checksum((uint8_t*)data,
                                    sizeof(*packet) - sizeof(packet->sum));
    }
  }

  if (cb != NULL) {
    uint32_t timeout_at_ms = millis() + wait_time;
    USER_DEBUG_PRINTF("timeout at %lu", timeout_at_ms);
    USER_DEBUG_PRINTF("millis = %lu", millis());
    while (millis() < timeout_at_ms) {
      USER_DEBUG_PRINTF("Waiting a command message...");
      delay(100);
    }
    if (this->_waiting_message_has_arrived==false) {
      USER_DEBUG_PRINTF("Timeout... %d", _waiting_message_has_arrived);
      cb();
    }
  }
}

void CMMC_ESPNow::on_message_recv(esp_now_recv_cb_t cb) {
  if (cb != NULL) {
    this->_user_on_message_recv = cb;
  }
}

void CMMC_ESPNow::on_message_sent(esp_now_send_cb_t cb) {
  if (cb != NULL) {
    this->_user_on_message_sent = cb;
  }
}

void CMMC_ESPNow::debug(cmmc_debug_cb_t cb) {
  if (cb!=NULL)
    this->_user_debug_cb = cb;
}

void CMMC_ESPNow::enable_retries(bool s) {
  this->_enable_retries = s;
}
