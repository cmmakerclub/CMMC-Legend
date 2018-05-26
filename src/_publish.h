#include <MqttConnector.h>

extern MqttConnector* mqtt;
extern char myName[];

static void readSensor(); 

extern String DEVICE_NAME;
extern int PUBLISH_EVERY;

extern uint32_t temperature;
extern uint32_t humidity;
extern uint32_t gas_resistance;
extern uint32_t pressure;

extern int bmeType;
extern int relayPinState;
extern int dhtType;

extern char sensorType[15];

void register_publish_hooks() {
  strcpy(myName, DEVICE_NAME.c_str());
  mqtt->on_prepare_data_once([&](void) {
    Serial.println("initializing sensor...");
  });

  mqtt->on_before_prepare_data([&](void) {
    readSensor();
  });

  mqtt->on_prepare_data([&](JsonObject *root) {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];
    data["appVersion"] = LEGEND_APP_VERSION;
    data["myName"] = myName;
    data["millis"] = millis(); 
    data["temperature_raw"] = temperature;
    data["humidity_raw"] = humidity;
    data["relayPinState"] = relayPinState;
    data["dhtType"] = dhtType;
    data["bmeType"] = bmeType;
    data["sensorType"] = sensorType;
    data["updateInterval"] = PUBLISH_EVERY;
  }, PUBLISH_EVERY);
  mqtt->on_after_prepare_data([&](JsonObject * root) {
    /**************
      JsonObject& data = (*root)["d"];
      data.remove("version");
      data.remove("subscription");
    **************/
  });
}

static void readSensor() {

}
