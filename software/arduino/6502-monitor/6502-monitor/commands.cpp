#include <Arduino.h>
#include <TimerThree.h>
#include "constants.h"
#include "colorize.h"

extern bool colorize;
extern bool int_enabled;
extern int clock_mode;
extern int clock_setting;

void clk_assert() {
  Serial.println("Controlling clock pin");
  digitalWrite(SBC_CLOCK, LOW);
  pinMode(SBC_CLOCK, OUTPUT);  
}

void clk_tick() {
  Serial.println("Tick!");
  digitalWrite(SBC_CLOCK, HIGH);
  delay(50);
  digitalWrite(SBC_CLOCK, LOW);
}

void clk_release() {
  Serial.println("Releasing clock pin");
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
    case ADR_VIA:
      ansi_weak();
      Serial.println(output);
      ansi_default();
      break;
    case ADR_VECTORS:
      ansi_error();
      Serial.println(output);
      ansi_default();
      break;
    case ADR_ZERO_PAGE:
    case ADR_STACK:
      ansi_notice();
      Serial.println(output);
      ansi_default();
      break;
    default:
      Serial.println(output);
      break;
  }
}

void int_attach() {
  if (!int_enabled) {
    int_enabled = true;
    Serial.println("Attaching interrupt");
    attachInterrupt(digitalPinToInterrupt(SBC_CLOCK), on_clock, RISING);
  }
}

void int_detach() {
  if (int_enabled) {
    int_enabled = false;
    Serial.println("Detaching interrupt");
    detachInterrupt(digitalPinToInterrupt(SBC_CLOCK));
  }
}

void pwm_enable() {
  Serial.println("Enable PWM");
  Timer3.pwm(SBC_CLOCK, 512);
}

void pwm_disable() {
  Serial.println("Disable PWM");
  Timer3.disablePwm(SBC_CLOCK);
}

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

void do_manual_clock() {
  set_clock_mode(CLK_MODE_MANUAL);

  Serial.print(F("Arduino clock now set to "));
  ansi_notice();
  Serial.print(F("MANUAL"));
  ansi_default();
  Serial.println();
}

int clock_speed() {
  return 1000000 / CLK_PERIOD[clock_setting];
}

void do_auto_clock() {
  set_clock_mode(CLK_MODE_AUTO);

  Serial.print(F("Arduino clock now set to "));
  ansi_notice();
  Serial.print(F("AUTOMATIC ("));
  Serial.print(clock_speed());
  Serial.print("Hz)");
  ansi_default();
  Serial.println();
}

void do_clock_disable() {
  set_clock_mode(CLK_MODE_NONE);

  Serial.print(F("Arduino clock now set to "));
  ansi_error();
  Serial.print(F("DISABLED"));
  ansi_default();
  Serial.println();
}

/* Reset SBC by holding w65c02 CPU in reset for 250ms. */
void do_reset() {
  ansi_bright();
  Serial.print("Resetting...");
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
}

/* Perform a manual clock pulse as long as clocking has been enabled. */
void do_tick() {
  if (clock_mode == CLK_MODE_NONE) {
    Serial.print(F("Arduino clock is "));
    ansi_error();
    Serial.print(F("DISABLED"));
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
    Serial.print(F("Arduino clock is "));
    ansi_error();
    Serial.print(F("DISABLED"));
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
      Serial.print(clock_speed());
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
      Serial.print(clock_speed());
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
  Serial.println(F("clock                 Print current clock settings"));
  Serial.println(F("clock <auto|manual>   Enables Arduino clock in manual or automatic mode"));
  Serial.println(F("help                  Prints this screen"));
  Serial.println(F("monitor <on|off>      Monitor updates"));
  Serial.println(F("reset                 Reset computer"));
  Serial.println(F("version               Prints 6502 Monitor version"));
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
  int_detach();
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
  if (handle_command(command, F("clock"), print_clock));
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
