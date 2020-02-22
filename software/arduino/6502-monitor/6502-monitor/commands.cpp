#include <Arduino.h>
#include <TimerThree.h>
#include "constants.h"
#include "typedefs.h"
#include "debug.h"
#include "ansi.h"
#include "clock.h"
#include "help.h"
#include "bus_control.h"

extern bool ansi_enabled;
extern volatile bool suppress_monitor;

extern int clock_mode;
extern int clock_setting;

int command_set = COMMAND_SET_MAIN;

void commands_init() {
  clock_initialize();
}

/*
 * Prints out the status of the 6502-system, this is done by checking wether
 * the reset lines has been pulled up. Might not be reliable as it'd be
 * floating though it somehow seems to be working anyway (famous last words
 * and all that).
 */
void print_status() {
  int val = digitalRead(SBC_RESET);
  Serial.print(F("Host system "));
  if (val == 0) {
    ansi_error_ln(F("offline"));
  }
  else {
    ansi_notice_ln(F("powered"));
  }
}

void print_version() {
  Serial.print(F("6502 Monitor "));
  Serial.println(VERSION);
}

void print_welcome() {
  ansi_clear();
  ansi_highlight();
  print_version();
  ansi_default();
}

/* Called when a recognized command has been recognized, but before the
 * function is actually called.
 */
void echo_command(String command) {
  switch (command_set) {
    case COMMAND_SET_MAIN:
    default:
      ansi_colour(COLOUR_CYAN);
      break;

    case COMMAND_SET_CONTROL:
      ansi_colour(COLOUR_MAGENTA);
      break;
  }
  Serial.println("> "+ command);
  ansi_default();
}

/* Called when the entered command has not been recognized, we don't know
 * what to do next so we'll just print it as an error instead.
 */
void echo_unknown(String command) {
  ansi_error();
  Serial.println("? " + command);
  ansi_default();
}

/* Clear the serial terminal screen, but note that this won't actually do
 * anything unless ANSI terminal codes are supported by the client and
 * have not been explicitly disabled. Does a second echo of the command
 * as the first one will disappear upon execution.
 */
void do_clear() {
  ansi_clear();
  if (ansi_enabled) echo_command(F("clear"));
}

/*
 * Specialized handler for when a period is sent via serial, but we need to
 * actually make sure that we're in a command mode where this is sensible.
 */
void do_tick() {
  if (command_set == COMMAND_SET_MAIN) {
    do_manual_tick();
  }
}

/*
 * Handle serial commands, mainly just matches the name and if it does the
 * supplied function is run. Recognized commands are echoed back to the user.
 */
bool handle_command(String command, String name, void (*function)(), bool suppress_echo = false) {
  if (command == name) {
    if (!suppress_monitor) {
      echo_command(command);
    }
    (*function)();
    return true;
  }
  return false;
}

void select_command_main(String command) {
       if (handle_command(command, F("ansi"), ansi_status));
  else if (handle_command(command, F("ansi on"), ansi_on));
  else if (handle_command(command, F("ansi off"), ansi_off));
  else if (handle_command(command, F("ansi test"), ansi_test));
  else if (handle_command(command, F("clear"), do_clear));
  else if (handle_command(command, F("clock"), print_clock));
  else if (handle_command(command, F("clock auto"), set_auto_clock));
  else if (handle_command(command, F("clock 1"), set_clock_1Hz));
  else if (handle_command(command, F("clock 2"), set_clock_2Hz));
  else if (handle_command(command, F("clock 4"), set_clock_4Hz));
  else if (handle_command(command, F("clock 16"), set_clock_16Hz));
  else if (handle_command(command, F("clock 32"), set_clock_32Hz));
  else if (handle_command(command, F("clock 128"), set_clock_128Hz));
  else if (handle_command(command, F("clock 256"), set_clock_256Hz));
  else if (handle_command(command, F("clock manual"), set_manual_clock));
  else if (handle_command(command, F("clock external"), set_external_clock));
  else if (handle_command(command, F("control"), set_control_on));
  else if (handle_command(command, F("help"), print_help));
  else if (handle_command(command, F("monitor on"), set_monitor_on));
  else if (handle_command(command, F("monitor off"), set_monitor_off));
  else if (handle_command(command, F("reset"), do_reset));
  else if (handle_command(command, F("status"), print_status));
  else if (handle_command(command, F("tick"), do_tick));
  else if (handle_command(command, F("version"), print_version));
  else {
    echo_unknown(command);
  }
}

void select_command_control(String command) {
       if (handle_command(command, F("clear"), do_clear));
  else if (handle_command(command, F("dump ram"), dump_ram));
  else if (handle_command(command, F("dump ram 1k"), dump_ram_1k));
  else if (handle_command(command, F("dump ram 2k"), dump_ram_2k));
  else if (handle_command(command, F("dump ram 4k"), dump_ram_4k));
  else if (handle_command(command, F("dump ram 8k"), dump_ram_8k));
  else if (handle_command(command, F("dump ram 16k"), dump_ram_16k));
  else if (handle_command(command, F("dump zp"), dump_zp));
  else if (handle_command(command, F("dump stack"), dump_stack));
  else if (handle_command(command, F("dump rom"), dump_rom));
  else if (handle_command(command, F("dump rom 1k"), dump_rom_1k));
  else if (handle_command(command, F("dump rom 2k"), dump_rom_2k));
  else if (handle_command(command, F("dump rom 4k"), dump_rom_4k));
  else if (handle_command(command, F("dump rom 8k"), dump_rom_8k));
  else if (handle_command(command, F("dump rom 16k"), dump_rom_16k));
  else if (handle_command(command, F("dump rom 32k"), dump_rom_32k));
  else if (handle_command(command, F("dump vectors"), dump_vectors));
  else if (handle_command(command, F("intel ram"), dump_intel_ram));
  else if (handle_command(command, F("intel ram 1k"), dump_intel_ram_1k));
  else if (handle_command(command, F("intel ram 2k"), dump_intel_ram_2k));
  else if (handle_command(command, F("intel ram 4k"), dump_intel_ram_4k));
  else if (handle_command(command, F("intel ram 8k"), dump_intel_ram_8k));
  else if (handle_command(command, F("intel ram 16k"), dump_intel_ram_16k));
  else if (handle_command(command, F("intel rom"), dump_intel_rom));
  else if (handle_command(command, F("intel rom 1k"), dump_intel_rom_1k));
  else if (handle_command(command, F("intel rom 2k"), dump_intel_rom_2k));
  else if (handle_command(command, F("intel rom 4k"), dump_intel_rom_4k));
  else if (handle_command(command, F("intel rom 8k"), dump_intel_rom_8k));
  else if (handle_command(command, F("intel rom 16k"), dump_intel_rom_16k));
  else if (handle_command(command, F("intel rom 32k"), dump_intel_rom_32k));
  else if (handle_command(command, F("intel stack"), dump_intel_stack));
  else if (handle_command(command, F("intel zp"), dump_intel_zp));
  else if (command.charAt(0) == ':') read_intel(command);
  else if (handle_command(command, F("paper ram"), dump_paper_ram));
  else if (handle_command(command, F("paper ram 1k"), dump_paper_ram_1k));
  else if (handle_command(command, F("paper ram 2k"), dump_paper_ram_2k));
  else if (handle_command(command, F("paper ram 4k"), dump_paper_ram_4k));
  else if (handle_command(command, F("paper ram 8k"), dump_paper_ram_8k));
  else if (handle_command(command, F("paper ram 16k"), dump_paper_ram_16k));
  else if (handle_command(command, F("paper rom"), dump_paper_rom));
  else if (handle_command(command, F("paper rom 1k"), dump_paper_rom_1k));
  else if (handle_command(command, F("paper rom 2k"), dump_paper_rom_2k));
  else if (handle_command(command, F("paper rom 4k"), dump_paper_rom_4k));
  else if (handle_command(command, F("paper rom 8k"), dump_paper_rom_8k));
  else if (handle_command(command, F("paper rom 16k"), dump_paper_rom_16k));
  else if (handle_command(command, F("paper rom 32k"), dump_paper_rom_32k));
  else if (handle_command(command, F("paper stack"), dump_paper_stack));
  else if (handle_command(command, F("paper zp"), dump_paper_zp));
  else if (command.charAt(0) == ';') read_paper(command);
  else if (handle_command(command, F("exit"), set_control_off));
  else if (handle_command(command, F("help"), print_help));
  else if (handle_command(command, F("test ram"), test_ram));
  else if (handle_command(command, F("test zp"), test_zp));
  else if (handle_command(command, F("test stack"), test_stack));
  else if (handle_command(command, F("zero zp"), zero_zp));
  else if (handle_command(command, F("zero stack"), zero_stack));
  else if (handle_command(command, F("zero ram"), zero_ram));
  else {
    echo_unknown(command);
  }
}

/*
 * Run the command associated with the serial command given, compared against
 * all known commands (depending on the currently selected command set).
 */
void select_command(String command) {
  switch (command_set) {
    case COMMAND_SET_MAIN:
    default:
      select_command_main(command);
      break;

    case COMMAND_SET_CONTROL:
      select_command_control(command);
      break;
  }
}

void select_command_main(int user_switch, switch_functions_t* r) {
  switch (user_switch) {
    case USER_SW1:
      r->short_press = nullptr;
      r->long_press = do_reset;
      return;
    
    case USER_SW2:
      r->short_press = do_tick;
      r->long_press = set_manual_clock;
      break;

    case USER_SW3:
      r->short_press = do_toggle_speed;
      r->long_press = set_auto_clock;
      break;
    }
}

/*
 * Called upon way too frequently by process switches, it is used to determine
 * the behavior of a specified switch before processing the state of it. This
 * isn't simply mapped statically as the active command set changes depending
 * on the mode used.
 */
void select_command(int user_switch, switch_functions_t* r) {
  switch (command_set) {
    case COMMAND_SET_MAIN:
      select_command_main(user_switch, r);
      break;

    default:
    case COMMAND_SET_CONTROL:
      r->short_press = nullptr;
      r->long_press = nullptr;
      break;
  }
}