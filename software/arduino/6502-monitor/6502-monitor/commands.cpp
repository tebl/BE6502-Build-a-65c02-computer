#include <Arduino.h>
#include <TimerThree.h>
#include "constants.h"
#include "debug.h"
#include "ansi.h"
#include "clock.h"

extern bool ansi_enabled;
extern volatile bool suppress_monitor;

extern int clock_mode;
extern int clock_setting;

void commands_init() {
  clock_initialize();
}

void print_help() {
  ansi_notice();
  Serial.println(F("Commands supported:"));
  ansi_default();
  Serial.println(F("ansi <on|off|test>    Control usage of terminal codes"));
  Serial.println(F("clear                 Clear screen"));
  Serial.println(F("clock                 Print current clock settings"));
  Serial.println(F("clock <auto|manual>   Enables Arduino clock in manual or automatic mode"));
  Serial.println(F("help                  Prints this screen"));
  Serial.println(F("monitor <on|off>      BUS monitor updates"));
  Serial.println(F("reset                 Reset computer"));
  Serial.println(F("version               Prints 6502 Monitor version"));
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

void echo_command(String command) {
  ansi_notice();
  Serial.println("> "+ command);
  ansi_default();
}

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
 * Handle serial commands, mainly just matches the name
 * and if it does the supplied function is run. Recognized
 * commands are echoed back to the user.
 */
bool handle_command(String command, String name, void (*function)()) {
  if (command == name) {
    echo_command(command);
    (*function)();
    return true;
  }
  return false;
}

/*
 * Run the command associated with the text command
 * given, if one is currently supported by the sketch.
 */
void select_command(String command) {
       if (handle_command(command, F("ansi"), ansi_status));
  else if (handle_command(command, F("ansi on"), ansi_on));
  else if (handle_command(command, F("ansi off"), ansi_off));
  else if (handle_command(command, F("ansi test"), ansi_test));
  else if (handle_command(command, F("clear"), do_clear));
  else if (handle_command(command, F("clock"), print_clock));
  else if (handle_command(command, F("clock auto"), do_auto_clock));
  else if (handle_command(command, F("clock manual"), do_manual_clock));
  else if (handle_command(command, F("clock disabled"), do_clock_disable));
  else if (handle_command(command, F("help"), print_help));
  else if (handle_command(command, F("monitor on"), set_monitor_on));
  else if (handle_command(command, F("monitor off"), set_monitor_off));
  else if (handle_command(command, F("reset"), do_reset));
  else if (handle_command(command, F("tick"), do_tick));
  else if (handle_command(command, F("version"), print_version));
  else {
    echo_unknown(command);
  }
}
