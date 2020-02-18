#include <Arduino.h>
#include "constants.h"
#include "ansi.h"

void debug(const char *string) {
  #ifdef DEBUG
  ansi_debug();
  Serial.println(string);
  ansi_default();
  #endif
}

void debug(const __FlashStringHelper *string) {
  #ifdef DEBUG
  ansi_debug();
  Serial.println(string);
  ansi_default();
  #endif
}