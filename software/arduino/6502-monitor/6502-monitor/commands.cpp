#include <Arduino.h>
#include <TimerThree.h>
#include "constants.h"
#include "colorize.h"

extern bool colorize;
extern bool output;
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
      clk_release();
    }
    if (clock_mode == CLK_MODE_AUTO) {
      pwm_disable();
      clk_release();
    }
    break;

  case CLK_MODE_MANUAL:
    if (clock_mode == CLK_MODE_NONE) {
      clk_assert();
    }
    if (clock_mode == CLK_MODE_AUTO) {
      pwm_disable();
    }
    break;

  case CLK_MODE_AUTO:
    if (clock_mode == CLK_MODE_NONE) {
      clk_assert();
      pwm_enable();
    }
    if (clock_mode == CLK_MODE_MANUAL) {
      pwm_enable();
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
      if (clock_setting > 5) clock_setting = 0;
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

/* From original sketch by Ben Eater, prints the values currently found on
 * the data and address bus. Usually called via interrupt.
 */
void on_clock() {
  if (!output) return;
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
  Serial.println(output);  
}

void print_help() {
  ansi_notice();
  Serial.println(F("Commands supported:"));
  ansi_default();
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
 */
void select_command(String command) {
  if (handle_command(command, F("clock auto"), do_auto_clock));
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
