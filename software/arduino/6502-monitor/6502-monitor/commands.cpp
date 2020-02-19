#include <Arduino.h>
#include <TimerThree.h>
#include "constants.h"
#include "debug.h"
#include "ansi.h"
#include "clock.h"
#include "bus_control.h"

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
  Serial.println(F("ansi on               Enable ANSI terminal code usage"));
  Serial.println(F("ansi off              Disable ANSI terminal code usage"));
  Serial.println(F("ansi test             Test ANSI terminal codes on terminal"));
  Serial.println(F("clear                 Clear screen"));
  Serial.println(F("clock                 Print current clock settings"));
  Serial.println(F("clock auto            Start or resume Arduino clock"));
  Serial.println(F("clock <speed>         Set Arduino clock in Hz (1,2,4,16,32,128,256)"));
  Serial.println(F("clock manual          Set Arduino clock to manual"));
  Serial.println(F("clock external        Disable Arduino clock options"));
  Serial.println(F("dump ram              Dump RAM"));
  Serial.println(F("dump rom              Dump ROM"));
  Serial.println(F("dump stack            Dump stack area"));
  Serial.println(F("dump vectors          Dump initialization vectors"));
  Serial.println(F("dump zp               Dump Zero-page"));
  Serial.println(F("help                  Prints this screen"));
  Serial.println(F("monitor <on|off>      BUS monitor updates"));
  Serial.println(F("reset                 Reset computer"));
  Serial.println(F("tick                  Manual clock tick, alternatively use '.'"));
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

/* Called when a recognized command has been recognized, but before the
 * function is actually called.
 */
void echo_command(String command) {
  ansi_notice();
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
  else if (handle_command(command, F("bus test"), do_bus_test));
  else if (handle_command(command, F("clear"), do_clear));
  else if (handle_command(command, F("clock"), print_clock));
  else if (handle_command(command, F("clock auto"), do_auto_clock));
  else if (handle_command(command, F("clock 1"), set_clock_1Hz));
  else if (handle_command(command, F("clock 2"), set_clock_2Hz));
  else if (handle_command(command, F("clock 4"), set_clock_4Hz));
  else if (handle_command(command, F("clock 16"), set_clock_16Hz));
  else if (handle_command(command, F("clock 32"), set_clock_32Hz));
  else if (handle_command(command, F("clock 128"), set_clock_128Hz));
  else if (handle_command(command, F("clock 256"), set_clock_256Hz));
  else if (handle_command(command, F("clock manual"), do_manual_clock));
  else if (handle_command(command, F("clock external"), do_clock_disable));
  else if (handle_command(command, F("dump ram"), dump_ram));
  else if (handle_command(command, F("dump zp"), dump_zero_page));
  else if (handle_command(command, F("dump stack"), dump_stack));
  else if (handle_command(command, F("dump rom"), dump_rom));
  else if (handle_command(command, F("dump vectors"), dump_vectors));
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
