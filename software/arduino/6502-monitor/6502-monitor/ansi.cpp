#include <Arduino.h>
#include "ansi.h"
extern bool ansi_enabled;

void ansi_clear() {
  if (ansi_enabled) {
    Serial.print(F("\033[H"));
    Serial.print(F("\033[2J"));
  }
}

void ansi_colour(int colour, bool bright = false) {
  if (ansi_enabled) {
    Serial.print(F("\033["));
    if (bright) Serial.print(F("1;"));
    Serial.print(colour);
    Serial.print("m");
  }
}

void ansi_decoration(int decoration = TEXT_DECORATION_PLAIN) {
  if (!ansi_enabled) return;
  Serial.print(F("\033["));
  Serial.print(decoration);
  Serial.print("m");
}

void ansi_default() {
  ansi_colour(COLOUR_RESET);
}

void ansi_highlight() {
  ansi_colour(COLOUR_WHITE, true);
  ansi_decoration(TEXT_DECORATION_UNDERLINE);
}

void ansi_debug() {
  ansi_colour(COLOUR_WHITE);
}

void ansi_error() {
  ansi_colour(COLOUR_RED, true);
  ansi_decoration(TEXT_DECORATION_BLINK);
}

void ansi_notice() {
  ansi_colour(COLOUR_CYAN);
}

void ansi_weak() {
  ansi_colour(COLOUR_BLUE);
}

void ansi_status() {
  if (ansi_enabled) {
    Serial.print(F("\033[1;31mA\033[1;32mN\033[1;33mS\033[1;34mI"));
    ansi_default();
    Serial.print(F(" terminal codes "));
    ansi_highlight();
    Serial.print(F("ON"));
    ansi_default();
    Serial.println();
  } else {
    Serial.println(F("ANSI terminal codes OFF"));
  }
}

void ansi_on() {
  ansi_enabled = true;
  ansi_status();
}

void ansi_off() {
  ansi_enabled = false;
  ansi_status();
}

void ansi_test() {
  ansi_clear();

  ansi_highlight();
  Serial.print("HIGHLIGHT");
  ansi_default();
  Serial.print(" ");

  ansi_debug();
  Serial.print("DEBUG");
  ansi_default();
  Serial.print(" ");

  ansi_error();
  Serial.print("ERROR");
  ansi_default();
  Serial.print(" ");

  ansi_notice();
  Serial.print("NOTICE");
  ansi_default();
  Serial.print(" ");

  ansi_weak();
  Serial.print("WEAK");
  ansi_default();
  Serial.println();

  ansi_decoration(TEXT_DECORATION_BOLD);
  Serial.print("BOLD");
  ansi_default();
  Serial.print(" ");
  ansi_decoration(TEXT_DECORATION_DIM);
  Serial.print("DIM");
  ansi_default();
  Serial.print(" ");
  ansi_decoration(TEXT_DECORATION_ITALIC);
  Serial.print("ITALIC");
  ansi_default();
  Serial.print(" ");
  ansi_decoration(TEXT_DECORATION_UNDERLINE);
  Serial.print("UNDERLINE");
  ansi_default();
  Serial.print(" ");
  ansi_decoration(TEXT_DECORATION_BLINK);
  Serial.print("BLINK");
  ansi_default();
  Serial.print(" ");
  ansi_decoration(TEXT_DECORATION_REVERSED);
  Serial.print("REVERSED");
  ansi_default();
  Serial.print(" ");
  ansi_decoration(TEXT_DECORATION_STRIKETHROUGH);
  Serial.print("STRIKETHROUGH");
  ansi_default();
  Serial.println();

  for (int i = COLOUR_BLACK; i <= COLOUR_WHITE; i++) {
    ansi_colour(i);
    Serial.print(i);
    Serial.print(" ");
  }
  ansi_default();
  Serial.println();

  for (int i = COLOUR_BLACK; i < COLOUR_WHITE; i++) {
    ansi_colour(i, true);
    Serial.print(i);
    Serial.print(" ");
  }
  ansi_default();
  Serial.println();

  for (int i = COLOUR_BG_BLACK; i <= COLOUR_BG_WHITE; i++) {
    ansi_colour(i);
    Serial.print(i);
    Serial.print(" ");
  }
  ansi_default();
  Serial.println();

  for (int i = COLOUR_BG_BLACK; i < COLOUR_BG_WHITE; i++) {
    ansi_colour(i, true);
    Serial.print(i);
    Serial.print(" ");
  }
  ansi_default();
  Serial.println();
}
