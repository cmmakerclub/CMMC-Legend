#include <Arduino.h>
#include <HardwareSerial.h>
#include <CMMC_Legend.h>
#include "WiFiModule.h"

CMMC_Legend *os;
extern HardwareSerial Serial;

void hook_init_ap(char* name, IPAddress ip) {
  // strcpy(ap_name, name);
  Serial.println("----------- hook_init_ap -----------");
  Serial.println(name);
  Serial.println(ip);
  Serial.println("/----------- hook_init_ap -----------");
}

void hook_button_pressed() {
  Serial.println("[user] hook_button_pressed");
}

void hook_button_released() {
  Serial.println("[user] hook_button_released");
}

void hook_button_long_pressed() {
  Serial.println("[user] hook_button_long_pressed");
}

// void hook_config_loaded();
// void hook_ready();
void hook_config_loaded() { }
void hook_ready() { }

void setup()
{
  Serial.begin(115200);
  static os_config_t config = {
    .BLINKER_PIN = 2,
    .BUTTON_MODE_PIN = 13,
    .SWITCH_PIN_MODE = INPUT_PULLUP,
    .SWITCH_PRESSED_LOGIC = LOW,
    .delay_after_init_ms = 200,
    .hook_init_ap = hook_init_ap,
    .hook_button_pressed =  hook_button_pressed,
    .hook_button_long_pressed = hook_button_long_pressed,
    .hook_button_released = hook_button_released,
    .hook_ready = hook_ready,
    .hook_config_loaded = hook_config_loaded,
  };


  os = new CMMC_Legend(&Serial);
  os->addModule(new WiFiModule());
  os->setup(&config);
  // Serial.printf("APP VERSION: %s\r\n", LEGEND_APP_VERSION);
}

void loop()
{
  os->run();
}
