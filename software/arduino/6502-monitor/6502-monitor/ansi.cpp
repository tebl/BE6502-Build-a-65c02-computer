#include <Arduino.h>
#include "ansi.h"
extern bool ansi_enabled;

/* 
 * Clear the screen and return cursor to top left position.
 */
void ansi_clear() {
  if (ansi_enabled) {
    Serial.print(F("\033[H"));
    Serial.print(F("\033[2J"));
  }
}

/* Set the specified colour as the current foreground colour, in order to
 * keep compatibility somewhat manageable only the basic 8 colours should
 * be used as well as the optional bright versions of them. A specified
 * background can be used instead, but the way this works is that 
 * foreground will remain locked at the default color so this may not be
 * all that practical.
 */
void ansi_colour(int colour, bool bright) {
  if (ansi_enabled) {
    Serial.print(F("\033["));
    if (bright) Serial.print(F("1;"));
    Serial.print(colour);
    Serial.print("m");
  }
}
void ansi_colour(const __FlashStringHelper *string, int colour, bool bright, bool back_to_default, bool newline) {
  ansi_colour(colour, bright);
  if (newline) Serial.println(string);
  else Serial.print(string);
  if (back_to_default) ansi_default();
}
void ansi_colour(const char *string, int colour, bool bright, bool back_to_default, bool newline) {
  ansi_colour(colour, bright);
  if (newline) Serial.println(string);
  else Serial.print(string);
  if (back_to_default) ansi_default();
}
void ansi_colour_ln(const __FlashStringHelper *string, int colour, bool bright, bool back_to_default) { ansi_colour(string, colour, bright, back_to_default, true); }
void ansi_colour_ln(const char *string, int colour, bool bright, bool back_to_default) { ansi_colour(string, colour, bright, back_to_default, true); }

/* 
 * Reset any formatting previously set, this will affect both color and any
 * applies styles.
 */
void ansi_default() { ansi_colour(COLOUR_RESET); }


/* Configure text decoration effects, but note that compatibily with actual
 * terminal software is very limited - mostly bold, underline and reversed
 * seem to work reliably.
 */
void ansi_decoration(int decoration) {
  if (!ansi_enabled) return;
  Serial.print(F("\033["));
  Serial.print(decoration);
  Serial.print("m");
}
void ansi_decoration(const __FlashStringHelper *string, int decoration, bool back_to_default, bool newline) {
  ansi_decoration(decoration);
  if (newline) Serial.println(string);
  else Serial.print(string);
  if (back_to_default) ansi_default();
}
void ansi_decoration_ln(const __FlashStringHelper *string, int decoration, bool back_to_default) { ansi_decoration(string, decoration, true); }

void ansi_highlight() {
  ansi_colour(COLOUR_WHITE, true);
  ansi_decoration(TEXT_DECORATION_UNDERLINE);
}
void ansi_highlight(const __FlashStringHelper *string, bool back_to_default, bool newline) {
  ansi_highlight();
  if (newline) Serial.println(string);
  else Serial.print(string);
  if (back_to_default) ansi_default();
}
void ansi_highlight_ln(const __FlashStringHelper *string, bool back_to_default) { ansi_highlight(string, back_to_default, true); }

void ansi_debug() { ansi_colour(COLOUR_WHITE); }
void ansi_debug(const __FlashStringHelper *string, bool back_to_default, bool newline) { ansi_colour(string, COLOUR_WHITE, false, back_to_default, newline); }
void ansi_debug_ln(const __FlashStringHelper *string, bool back_to_default) { ansi_colour(string, COLOUR_WHITE, false, back_to_default, true); }
void ansi_debug(const char *string, bool back_to_default, bool newline) { ansi_colour(string, COLOUR_WHITE, false, back_to_default, newline); }
void ansi_debug_ln(const char *string, bool back_to_default) { ansi_colour(string, COLOUR_WHITE, false, back_to_default, true); }

void ansi_error() { ansi_colour(COLOUR_RED, true); }
void ansi_error(const __FlashStringHelper *string, bool back_to_default, bool newline) { ansi_colour(string, COLOUR_RED, true, back_to_default, newline); }
void ansi_error_ln(const __FlashStringHelper *string, bool back_to_default) { ansi_colour(string, COLOUR_RED, true, back_to_default, true); }
void ansi_error(const char *string, bool back_to_default, bool newline) { ansi_colour(string, COLOUR_RED, true, back_to_default, newline); }
void ansi_error_ln(const char *string, bool back_to_default) { ansi_colour(string, COLOUR_RED, true, back_to_default, true); }

void ansi_notice() { ansi_colour(COLOUR_CYAN); }
void ansi_notice(const __FlashStringHelper *string, bool back_to_default, bool newline) { ansi_colour(string, COLOUR_CYAN, false, back_to_default, newline); }
void ansi_notice_ln(const __FlashStringHelper *string, bool back_to_default) { ansi_colour(string, COLOUR_CYAN, false, back_to_default, true); }
void ansi_notice(const char *string, bool back_to_default, bool newline) { ansi_colour(string, COLOUR_CYAN, false, back_to_default, newline); }
void ansi_notice_ln(const char *string, bool back_to_default) { ansi_colour(string, COLOUR_CYAN, false, back_to_default, true); }

void ansi_weak() { ansi_colour(COLOUR_BLUE); }
void ansi_weak(const __FlashStringHelper *string, bool back_to_default, bool newline) { ansi_colour(string, COLOUR_BLUE, false, back_to_default, newline); }
void ansi_weak_ln(const __FlashStringHelper *string, bool back_to_default) { ansi_colour(string, COLOUR_BLUE, false, back_to_default, true); }
void ansi_weak(const char *string, bool back_to_default, bool newline) { ansi_colour(string, COLOUR_BLUE, false, back_to_default, newline); }
void ansi_weak_ln(const char *string, bool back_to_default) { ansi_colour(string, COLOUR_BLUE, false, back_to_default, true); }

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

/* Test ANSI control codes by writing them out to the serial terminal,
 * obviously won't work when the codes are deactivated. Verification will
 * be up to the user to determine. 
 */
void ansi_test() {
  ansi_clear();

  ansi_highlight(F("HIGHLIGHT"));
  Serial.print(" ");

  ansi_debug(F("DEBUG"));
  Serial.print(" ");

  ansi_error(F("ERROR"));
  Serial.print(" ");

  ansi_notice(F("NOTICE"));
  Serial.print(" ");

  ansi_weak(F("WEAK"));
  Serial.println();

  ansi_decoration(F("BOLD"), TEXT_DECORATION_BOLD);
  Serial.print(" ");

  ansi_decoration(F("DIM"), TEXT_DECORATION_DIM);
  Serial.print(" ");

  ansi_decoration(F("ITALIC"), TEXT_DECORATION_ITALIC);
  Serial.print(" ");

  ansi_decoration(F("UNDERLINE"), TEXT_DECORATION_UNDERLINE);
  Serial.print(" ");

  ansi_decoration(F("BLINK"), TEXT_DECORATION_BLINK);
  Serial.print(" ");

  ansi_decoration(F("REVERSED"), TEXT_DECORATION_REVERSED);
  Serial.print(" ");

  ansi_decoration(F("STRIKETHROUGH"), TEXT_DECORATION_STRIKETHROUGH);
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
