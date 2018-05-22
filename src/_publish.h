#include <MqttConnector.h>

extern MqttConnector* mqtt;
extern char myName[];

static void readSensor(); 

extern String DEVICE_NAME;
extern int PUBLISH_EVERY;

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
  // perform reading sensor 
  Serial.println("Perform reading and publishing senso data.");
}
