#include <MqttConnector.h>

extern MqttConnector* mqtt;
extern char myName[];

static void readSensor(); 

extern String DEVICE_NAME;
extern int PUBLISH_EVERY;

extern uint32_t gas_resistance;
extern uint32_t pressure;

extern int bmeType;
extern int relayPinState;
extern int dhtType;
extern CMMC_SENSOR_DATA_T sensorData;

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
    data["relayPinState"] = relayPinState;
    data["dhtType"] = dhtType;
    data["bmeType"] = bmeType;
    data["sensorType"] = sensorType;
    data["updateInterval"] = PUBLISH_EVERY;
    if (sensorData.field1) data["field1"] = sensorData.field1;
    if (sensorData.field2) data["field2"] = sensorData.field2;
    if (sensorData.field3) data["field3"] = sensorData.field3;
    if (sensorData.field4) data["field4"] = sensorData.field4;
    if (sensorData.field5) data["field5"] = sensorData.field5;
    if (sensorData.field6) data["field6"] = sensorData.field6;
    if (sensorData.field7) data["field7"] = sensorData.field7;
    if (sensorData.field8) data["field8"] = sensorData.field8;
    if (sensorData.ms) data["ms"] = sensorData.ms;
    if (sensorData.battery) data["battery"] = sensorData.battery;

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
