#include <Arduino.h>
extern bool colorize;

void ansi_default() {
  if (colorize) Serial.print(F("\033[0m"));
}

void ansi_bright() {
  if (colorize) Serial.print(F("\033[1;37m"));
}

void ansi_weak() {
  if (colorize) Serial.print(F("\033[34m"));
}

void ansi_notice() {
  if (colorize) Serial.print(F("\033[36m"));
}

void ansi_error() {
  if (colorize) Serial.print(F("\033[1;31m"));
}

void ansi_on() {
  colorize = true;
  Serial.print(F("\033[1;31mA\033[1;32mN\033[1;33mS\033[1;34mI"));
  ansi_default();
  Serial.print(F(" terminal codes "));
  ansi_bright();
  Serial.print(F("ON"));
  ansi_default();
  Serial.println(F("!"));
}

void ansi_off() {
  colorize = false;
  Serial.println(F("ANSI terminal codes OFF!"));
}
