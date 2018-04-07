#include <Arduino.h>

#include "CMMCEasy.h"
CMMCEasy easy;
CMMC_Interval ti;

void setup() {
  Serial.begin(115200);
  Serial.println("OK");

  pinMode(LED_BUILTIN, OUTPUT);

  easy.blinker.init(CMMC_Blink::TYPE_TICKER);
  easy.blinker.blink(500, LED_BUILTIN);
  delay(3000);
  easy.blinker.blink(20, LED_BUILTIN);
  delay(2000);
  easy.blinker.blink(500, LED_BUILTIN);
  delay(1000);
  easy.blinker.blink(100, LED_BUILTIN);
  delay(1000);
  easy.blinker.blink(1000);
  delay(1000);
  easy.blinker.blink(100);

  }


void loop() {
  int outside = 0;
  static int outside_static = 0;

  ti.every_ms(222, [&]() {
    outside++;
    outside_static++;
    Serial.printf("IN SIDE %lu \n", millis());
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  });

  delay(1000);
  Serial.printf("outside = %d, outside_static = %d\n", outside, outside_static);

}
