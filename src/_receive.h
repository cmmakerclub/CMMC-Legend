#include <Arduino.h>
#include <MqttConnector.h>
#include <FS.h>

extern String MQTT_CLIENT_ID;
extern String MQTT_PREFIX;

extern char myName[];
extern uint32_t lastRecv; 
extern CMMC_Gpio gpio;

void register_receive_hooks(MqttConnector *mqtt) {
  mqtt->on_subscribe([&](MQTT::Subscribe *sub) -> void {
    Serial.printf("onSubScribe myName = %s \r\n", myName);
    sub->add_topic(MQTT_PREFIX + myName + String("/$/+"));
    sub->add_topic(MQTT_PREFIX + MQTT_CLIENT_ID + String("/$/+"));
    sub->add_topic(MQTT_PREFIX + myName + String("/status"));
    Serial.println("done on_subscribe...");
  });

  mqtt->on_before_message_arrived_once([&](void) { });

  mqtt->on_message([&](const MQTT::Publish & pub) { });

  mqtt->on_after_message_arrived([&](String topic, String cmd, String payload) {
    // Serial.printf("recv topic: %s\r\n", topic.c_str());
    // Serial.printf("recv cmd: %s\r\n", cmd.c_str()); 
    // Serial.printf("payload: %s\r\n", payload.c_str());
    if (cmd == "$/command") {
      if (payload == "ON") {
        Serial.println("ON");
        gpio.on();
        // relayPinState = 1;
      }
      else if (payload == "OFF") {
        // relayPinState = 0;
        gpio.off();
        Serial.println("OFF");
      }
      else if (payload == "FORCE_CONFIG") {
        SPIFFS.remove("/enabled"); 
        ESP.restart();
      }
    }
    else if (cmd == "$/reboot") {
      ESP.restart();
    }
    else if (cmd == "status") {
      // Serial.println("sent & recv.");
    }
    else {
      Serial.println("Another message arrived.");
      // another message.
    }
    lastRecv = millis();
  });
}
