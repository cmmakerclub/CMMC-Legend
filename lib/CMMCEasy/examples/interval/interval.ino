#include <Arduino.h>
#include "CMMCEasy.h"

CMMCEasy easy;
CMMC_Interval &interval = easy.interval;

void setup() {
  Serial.begin(115200);
}

void loop() {
  interval.every_ms(301, [&]() {
    Serial.printf("M = %lu\n", millis());
  });
}
