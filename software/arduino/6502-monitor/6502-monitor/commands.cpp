#include <Arduino.h>
#include <TimerThree.h>
#include "constants.h"
#include "debug.h"
#include "ansi.h"

extern bool ansi_enabled;
extern bool int_enabled;
extern volatile bool suppress_monitor;
extern int clock_mode;
extern int clock_setting;

void commands_init() {
  Timer3.initialize(CLK_PERIOD[clock_setting]);
}

void clk_assert() {
  debug(F("Controlling clock pin"));
  digitalWrite(SBC_CLOCK, LOW);
  pinMode(SBC_CLOCK, OUTPUT);  
}

void clk_tick() {
  debug(F("Tick!"));
  digitalWrite(SBC_CLOCK, HIGH);
  delay(50);
  digitalWrite(SBC_CLOCK, LOW);
}

void clk_release() {
  debug(F("Releasing clock pin"));
  pinMode(SBC_CLOCK, INPUT);  
}

int address_segment(unsigned int address) {
  if (address >= 0xFFFA) return ADR_VECTORS;
  if (address >= 0x8000) return ADR_ROM;
  if (address >= 0x6000) return ADR_VIA;
  if (address >= 0x4000) return ADR_CUSTOM;
  if (address <= 0x00FF) return ADR_ZERO_PAGE;
  if (address <= 0x01FF) return ADR_STACK;
  return ADR_RAM;
}

/* From original sketch by Ben Eater, prints the values currently found on
 * the data and address bus. Usually called via interrupt.
 */
void on_clock() {
  if (suppress_monitor) return;
  char output[15];

  unsigned int address = 0;
  for (int n = 0; n < 16; n += 1) {
    int bit = digitalRead(SBC_ADDR[n]) ? 1 : 0;
    Serial.print(bit);
    address = (address << 1) + bit;
  }
  
  Serial.print("   ");
  
  unsigned int data = 0;
  for (int n = 0; n < 8; n += 1) {
    int bit = digitalRead(SBC_DATA[n]) ? 1 : 0;
    Serial.print(bit);
    data = (data << 1) + bit;
  }

  sprintf(output, "   %04X  %c %02X", address, digitalRead(SBC_RW) ? 'R' : 'W', data);
  switch(address_segment(address)) {
    case ADR_VECTORS:
      ansi_colour(COLOUR_RED, true);
      break;
    case ADR_ROM:
      ansi_colour(COLOUR_MAGENTA);
      break;
    case ADR_CUSTOM:
      ansi_colour(COLOUR_BLUE, true);
      break;
    case ADR_VIA:
      ansi_colour(COLOUR_BLUE);
      break;
    case ADR_STACK:
      ansi_colour(COLOUR_CYAN);
      break;
    case ADR_ZERO_PAGE:
      ansi_colour(COLOUR_CYAN, true);
      break;
    default:
      break;
  }
  Serial.println(output);
  ansi_default();
}

void int_attach() {
  if (!int_enabled) {
    int_enabled = true;
    debug(F("Attaching interrupt"));
    attachInterrupt(digitalPinToInterrupt(SBC_CLOCK), on_clock, RISING);
  }
}

void int_detach() {
  if (int_enabled) {
    int_enabled = false;
    debug(F("Detaching interrupt"));
    detachInterrupt(digitalPinToInterrupt(SBC_CLOCK));
    delay(50);
  }
}

void pwm_enable() {
  debug(F("Enable PWM"));
  Timer3.pwm(SBC_CLOCK, 512);
}

void pwm_disable() {
  debug(F("Disable PWM"));
  Timer3.disablePwm(SBC_CLOCK);
}

/* Handles the transition between the various clock modes, making sure to
 * enable and disable functionality as needed. 
 */
void set_clock_mode(int new_mode) {
  switch (new_mode) {
  case CLK_MODE_NONE:
    if (clock_mode == CLK_MODE_MANUAL) {
      int_detach();
      clk_release();
    }
    if (clock_mode == CLK_MODE_AUTO) {
      int_detach();
      pwm_disable();
      clk_release();
    }
    break;

  case CLK_MODE_MANUAL:
    if (clock_mode == CLK_MODE_NONE) {
      clk_assert();
      int_attach();
    }
    if (clock_mode == CLK_MODE_AUTO) {
      pwm_disable();
      int_attach();
    }
    break;

  case CLK_MODE_AUTO:
    if (clock_mode == CLK_MODE_NONE) {
      clk_assert();
      pwm_enable();
      if (clock_setting < CLK_MAX_MONITOR_SPEED) int_attach();
      else int_detach();
    }
    if (clock_mode == CLK_MODE_MANUAL) {
      pwm_enable();
      if (clock_setting < CLK_MAX_MONITOR_SPEED) int_attach();
      else int_detach();
    }
    break;
  }
  clock_mode = new_mode;
}

/* Set clock to manual */
void do_manual_clock() {
  set_clock_mode(CLK_MODE_MANUAL);

  Serial.print(F("Arduino clock now set to "));
  ansi_weak();
  Serial.print(F("MANUAL"));
  ansi_default();
  Serial.println();
}

/* Calculate clock speed in Hz */
int delay_to_hz() {
  return 1000000 / CLK_PERIOD[clock_setting];
}

/* Set clock to automatic */
void do_auto_clock() {
  set_clock_mode(CLK_MODE_AUTO);

  Serial.print(F("Arduino clock now set to "));
  ansi_weak();
  Serial.print(F("AUTOMATIC ("));
  Serial.print(delay_to_hz());
  Serial.print("Hz)");
  ansi_default();
  Serial.println();
}

/* Disable clock and set it back to external */
void do_clock_disable() {
  set_clock_mode(CLK_MODE_NONE);

  Serial.print(F("Arduino clock now set to "));
  ansi_weak();
  Serial.print(F("EXTERNAL"));
  ansi_default();
  Serial.println();
}

/* Reset SBC by holding w65c02 CPU in reset for 250ms if the clock is set to
 * external, meaning we have no idea what it is running it as so we just
 * assume that it is fast. If a manual clock or auto clock is specified for
 * use then we follow the datasheet and hold reset for two clock cycles.
 */
void do_reset() {
  int current_mode = clock_mode;

  switch (clock_mode) {
    case CLK_MODE_AUTO:
      set_clock_mode(CLK_MODE_MANUAL);
    case CLK_MODE_MANUAL:
      ansi_highlight();
      suppress_monitor = true;
      Serial.print("Doing controlled reset..."); 
      ansi_default();
      digitalWrite(SBC_RESET, HIGH);
      pinMode(SBC_RESET, OUTPUT);
      digitalWrite(SBC_RESET, LOW);

      clk_tick();
      clk_tick();

      digitalWrite(SBC_RESET, HIGH);
      pinMode(SBC_RESET, INPUT);
      set_clock_mode(current_mode);

      ansi_weak();
      Serial.println(" done!");
      suppress_monitor = false;
      ansi_default();
      break;

    default:
      ansi_highlight();
      Serial.print("Doing timed reset...");
      ansi_default();
      digitalWrite(SBC_RESET, HIGH);
      pinMode(SBC_RESET, OUTPUT);
      digitalWrite(SBC_RESET, LOW);
      delay(250);
      digitalWrite(SBC_RESET, HIGH);
      pinMode(SBC_RESET, INPUT);
      ansi_weak();
      Serial.println(" done!");
      ansi_default();
      break;
  }
}

/* Perform a manual clock pulse as long as clocking has been enabled. */
void do_tick() {
  if (clock_mode == CLK_MODE_NONE) {
    Serial.print(F("Arduino clock is set to "));
    ansi_error();
    Serial.print(F("EXTERNAL"));
    ansi_default();
    Serial.println();
  }

  if (clock_mode == CLK_MODE_AUTO) do_manual_clock();
  if (clock_mode == CLK_MODE_MANUAL) {
    clk_tick();
  }
}

void do_toggle_speed() {
  if (clock_mode == CLK_MODE_NONE) {
    Serial.print(F("Arduino clock is set to "));
    ansi_error();
    Serial.print(F("EXTERNAL"));
    ansi_default();
    Serial.println();
  }

  if (clock_mode == CLK_MODE_MANUAL) do_auto_clock();
  else {
    if (clock_mode == CLK_MODE_AUTO) {
      clock_setting++;
      if (clock_setting > CLK_MAX_SETTING) clock_setting = 0;
      if (clock_setting < CLK_MAX_MONITOR_SPEED) int_attach();
      else int_detach();
      Timer3.setPeriod(CLK_PERIOD[clock_setting]);

      Serial.print("Arduino clock speed set to ");
      ansi_notice();
      Serial.print(delay_to_hz());
      Serial.print("Hz");
      ansi_default();
      Serial.println();
    }
  }
}

void print_clock() {
  switch (clock_mode) {
    case CLK_MODE_NONE:
      Serial.print(F("Arduino clock is set to "));
      ansi_weak();
      Serial.print(F("EXTERNAL"));
      ansi_default();
      Serial.println();
      break;
    case CLK_MODE_MANUAL:
      Serial.print(F("Arduino clock is set to "));
      ansi_weak();
      Serial.print(F("MANUAL"));
      ansi_default();
      Serial.println();
      break;
    case CLK_MODE_AUTO:
      Serial.print(F("Arduino clock is set to "));
      ansi_weak();
      Serial.print(F("AUTO ("));
      Serial.print(delay_to_hz());
      Serial.print("Hz)");
      ansi_default();
      Serial.println();
      break;
  }
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

void set_monitor_off() {
  int_detach();
  Serial.print(F("Monitor output "));
  ansi_weak();
  Serial.print(F("OFF"));
  ansi_default();
  Serial.println();
}

void set_monitor_on() {
  Serial.print(F("Monitor output "));
  ansi_highlight();
  Serial.print(F("ON"));
  ansi_default();
  Serial.println();
  int_attach();
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
