#include <Arduino.h>
#include "constants.h"
#include "colorize.h"
extern bool colorize;
extern bool output;

void print_help() {
  ansi_notice();
  Serial.println(F("Commands supported:"));
  ansi_default();
  Serial.println(F("help              Prints this screen"));
  Serial.println(F("monitor <on|off>  Monitor updates"));
  Serial.println(F("version           Prints 6502 Monitor version"));
}

void print_version() {
  Serial.print(F("6502 Monitor "));
  Serial.println(VERSION);
}

void print_welcome() {
  ansi_bright();
  print_version();
  ansi_default();
}

void set_monitor_off() {
  output = false;
  Serial.print(F("Monitor output "));
  ansi_weak();
  Serial.print(F("OFF"));
  ansi_default();
  Serial.println();
}

void set_monitor_on() {
  Serial.print(F("Monitor output "));
  ansi_notice();
  Serial.print(F("ON"));
  ansi_default();
  Serial.println();
  output = true;
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
 * Note that Intel HEX (:<data>) and Paper-tape
 * (;<data>) are handled on a line-by-line basis instead
 * of as a complete listing to keep things easy.
 */
void select_command(String command) {
  if (handle_command(command, F("help"), print_help));
  else if (handle_command(command, F("monitor on"), set_monitor_on));
  else if (handle_command(command, F("monitor off"), set_monitor_off));
  else if (handle_command(command, F("version"), print_version));
  else {
    echo_unknown(command);
  }
}