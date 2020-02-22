#include <Arduino.h>
#include "constants.h"
#include "ansi.h"

extern int command_set;

/*
 * Calculates the length of a string that has been stored in Flash memory
 * instead of RAM, normally defined using the Arduino F() macro.
 */
unsigned int flash_length(const __FlashStringHelper *string) {
  const char *ptr = (const char *) string;
  int size = 0;
  while (pgm_read_byte_near(ptr++)) {
    size++;
	}
  return size;
}

/*
 * Arduino lacks a way to pad strings, so then I guess we're doing it the 
 * ridiculously hard way instead of leaving things somewhat sensible. Feature
 * creep is a real thing, but doing this for fun, so let's just live with it!
 */
void help_command(const __FlashStringHelper *command = nullptr, 
                  const __FlashStringHelper *description = nullptr,
                  const __FlashStringHelper *parameter = nullptr,
                  const __FlashStringHelper *switch_ref = nullptr) {
  int size = 0;
  int position = 0;
  if (command != nullptr) {
    size = flash_length(command);
    ansi_notice(command);
  }

  if (parameter != nullptr) {
    size += flash_length(parameter) + 1;
    Serial.print(' ');
    ansi_colour(parameter, COLOUR_WHITE);
  }
  for (int i = size; i < 20; i++) Serial.print(' ');
  position = 20;

  if (description != nullptr) {
    ansi_colour(description, COLOUR_WHITE);
    position += flash_length(description);
  }

  if (switch_ref != nullptr) {
    size = TERMINAL_COLS - flash_length(switch_ref);
    for (int i = position; i < size; i++) Serial.print(' ');
    ansi_colour(switch_ref, COLOUR_MAGENTA);
  }

  Serial.println();
}

void print_help_main() {
  ansi_colour_ln(F("Commands supported:"), COLOUR_MAGENTA);
  help_command(F("ansi on"),        F("Enable ANSI terminal code usage"));
  help_command(F("ansi off"),       F("Disable ANSI terminal code usage"));
  help_command(F("ansi test"),      F("Test ANSI terminal codes on terminal"));
  help_command(F("clear"),          F("Clear screen"));
  help_command(F("clock"),          F("Print current clock settings"));
  help_command(F("clock auto"),     F("Enable automatic Arduino clock, this will unlock fast "),   nullptr,        F("SW3*"));
  help_command(nullptr,             F("switching between manual and automatic mode. Holding "));
  help_command(nullptr,             F("SW3 on Mega Adapter will do the same thing (one flash"));
  help_command(nullptr,             F("and then two more)"));
  help_command(F("clock"),          F("Set Arduino clock in Hz (1,2,4,16,32,128,256). Pushing"),   F("<speed>"),   F("SW3 "));
  help_command(nullptr,             F("SW3 will toggle between the various speeds."));
  help_command(F("clock manual"),   F("Set Arduino clock to manual"),                              nullptr,        F("SW2*"));
  help_command(F("clock external"), F("Disable Arduino clock options"));
  help_command(F("control"),        F("Enter bus control mode"));
  help_command(F("help"),           F("Prints this screen"));
  help_command(F("monitor"),        F("BUS monitor updates"),                                      F("<on|off>"));
  help_command(F("reset"),          F("Reset computer depending on clock mode selected, hold "),   nullptr,        F("SW1*"));
  help_command(nullptr,             F("down SW1 for same function. Will reset for two cycles"));
  help_command(nullptr,             F("when Arduino controls clock, otherwise 250ms as usual."));
  help_command(F("status"),         F("Print system status"));
  help_command(F("tick"),           F("Manual clock tick, alternatively a period in the serial"),  nullptr,        F("SW2 "));
  help_command(nullptr,             F("terminal or pushing SW2 will perform the same task."));
  help_command(F("version"),        F("Prints 6502 Monitor version"));
}

void print_help_control() {
  ansi_colour_ln(F("Controls supported:"), COLOUR_MAGENTA);
  help_command(F("exit"),           F("Exit bus control mode"));
  help_command(F("clear"),          F("Clear screen"));
  help_command(F("dump ram"),       F("Dump RAM"));
  help_command(F("dump ram"),       F("Limited RAM dump (1k, 2k, 4k, 8k or 16k)"),                 F("<limit>"));
  help_command(F("dump rom"),       F("Dump ROM"));
  help_command(F("dump rom"),       F("Limited ROM dump (1k, 2k, 4k, 8k, 16k or 32k)"),            F("<limit>"));
  help_command(F("dump stack"),     F("Dump stack area"));
  help_command(F("dump vectors"),   F("Dump initialization vectors"));
  help_command(F("dump zp"),        F("Dump Zero-page"));
  help_command(F("help"),           F("Prints this screen"));
  help_command(F("intel ram"),      F("Dump RAM (Intel HEX format)"));
  help_command(F("intel ram"),      F("Limited RAM dump (1k, 2k, 4k, 8k or 16k)"),                 F("<limit>"));
  help_command(F("intel rom"),      F("Dump ROM (Intel HEX format)"));
  help_command(F("intel rom"),      F("Limited ROM dump (1k, 2k, 4k, 8k, 16k or 32k)"),            F("<limit>"));
  help_command(F("intel stack"),    F("Dump stack area (Intel HEX format)"));
  help_command(F("intel zp"),       F("Dump Zero-page (Intel HEX format)"));
  help_command(F(":<record>"),      F("Process Intel HEX record (pasted from HEX-file)"));
  help_command(F("paper ram"),      F("Dump RAM (Paper tape format)"));
  help_command(F("paper ram"),      F("Limited RAM dump (1k, 2k, 4k, 8k or 16k)"),                 F("<limit>"));
  help_command(F("paper rom"),      F("Dump ROM (Paper tape format)"));
  help_command(F("paper rom"),      F("Limited ROM dump (1k, 2k, 4k, 8k, 16k or 32k)"),            F("<limit>"));
  help_command(F("paper stack"),    F("Dump stack area (Paper tape format)"));
  help_command(F("paper zp"),       F("Dump Zero-page (Paper tape format)"));
  help_command(F(";<record>"),      F("Process paper tape record (pasted from file)"));
  help_command(F("test ram"),       F("RAM memory test"));
  help_command(F("zero zp"),        F("Zero page 0"));
  help_command(F("zero stack"),     F("Zero stack area"));
  help_command(F("zero ram"),       F("Zero RAM"));
}

/*
 * Run the command associated with the serial command given, compared against
 * all known commands (depending on the currently selected command set).
 */
void print_help() {
  if (command_set == COMMAND_SET_CONTROL) {
    print_help_control();
  } else {
    print_help_main();
  }
}