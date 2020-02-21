#include <Arduino.h>
#include <TimerThree.h>
#include "constants.h"
#include "debug.h"
#include "ansi.h"

extern bool int_enabled;
extern volatile bool suppress_monitor;

extern int clock_mode;
extern int clock_setting;

void clock_initialize() {
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

/* Translates supplied address according to the various blocks set aside
 * on the system memory map.
 */
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
 * the data and address bus - with additions for colour formatting entries
 * based on the address. Called via an interrupt called on the rising edge
 * of the system clock
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
      if (clock_setting <= CLK_MAX_MONITOR_SPEED) int_attach();
      else int_detach();
    }
    if (clock_mode == CLK_MODE_MANUAL) {
      pwm_enable();
      if (clock_setting <= CLK_MAX_MONITOR_SPEED) int_attach();
      else int_detach();
    }
    break;
  }
  clock_mode = new_mode;
}

/* Set clock to manual */
void set_manual_clock() {
  set_clock_mode(CLK_MODE_MANUAL);

  Serial.print(F("Arduino clock now set to "));
  ansi_weak_ln(F("MANUAL"));
}

/* Calculate clock speed in Hz */
int delay_to_hz() {
  return 1000000 / CLK_PERIOD[clock_setting];
}

/* Set clock to automatic */
void set_auto_clock() {
  set_clock_mode(CLK_MODE_AUTO);

  Serial.print(F("Arduino clock now set to "));
  ansi_weak(F("AUTOMATIC ("));
  Serial.print(delay_to_hz());
  ansi_weak_ln(F("Hz)"));
}

/* Disable clock and set it back to external */
void set_external_clock() {
  set_clock_mode(CLK_MODE_NONE);

  Serial.print(F("Arduino clock now set to "));
  ansi_weak_ln(F("EXTERNAL"));
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
      suppress_monitor = true;
      ansi_notice(F("Doing controlled reset..."));
      digitalWrite(SBC_RESET, HIGH);
      pinMode(SBC_RESET, OUTPUT);
      digitalWrite(SBC_RESET, LOW);

      clk_tick();
      clk_tick();

      digitalWrite(SBC_RESET, HIGH);
      pinMode(SBC_RESET, INPUT);
      set_clock_mode(current_mode);

      ansi_weak_ln(F(" done!"));
      suppress_monitor = false;
      break;

    default:
      ansi_notice(F("Doing timed reset..."));
      digitalWrite(SBC_RESET, HIGH);
      pinMode(SBC_RESET, OUTPUT);
      digitalWrite(SBC_RESET, LOW);
      delay(250);
      digitalWrite(SBC_RESET, HIGH);
      pinMode(SBC_RESET, INPUT);
      ansi_weak_ln(F(" done!"));
      break;
  }
}

/* Perform a manual clock pulse as long as clocking has been enabled. The
 * function will also be used when a period character is sent via the
 * serial interface.
 */
void do_manual_tick() {
  if (clock_mode == CLK_MODE_NONE) {
    Serial.print(F("Arduino clock is set to "));
    ansi_error_ln(F("EXTERNAL"));
  }

  if (clock_mode == CLK_MODE_AUTO) set_manual_clock();
  if (clock_mode == CLK_MODE_MANUAL) {
    clk_tick();
  }
}

/* Toggle Arduino clock speed as long as the Arduino is in charge of it, ie.
 * it's not under control by the external clock. If we're manually clocking
 * the function will automatically switch to automatic mode using previously
 * used speed. 
 */
void set_clock_speed(int new_setting) {
  if (clock_mode == CLK_MODE_NONE) {
    Serial.print(F("Arduino clock is set to "));
    ansi_error();
    Serial.print(F("EXTERNAL"));
    ansi_default();
    Serial.println();
  } else {
    clock_setting = new_setting;
    
    if (clock_mode == CLK_MODE_MANUAL) {
      Timer3.setPeriod(CLK_PERIOD[clock_setting]);
      set_auto_clock();
    }

    if (clock_mode == CLK_MODE_AUTO) {
      Timer3.setPeriod(CLK_PERIOD[clock_setting]);
      if (clock_setting <= CLK_MAX_MONITOR_SPEED) int_attach();
      else int_detach();

      Serial.print("Arduino clock speed set to ");
      ansi_notice();
      Serial.print(delay_to_hz());
      Serial.print("Hz");
      ansi_default();
      Serial.println();
    }
  }
}
void set_clock_1Hz() { set_clock_speed(CLK_SPEED_1); }
void set_clock_2Hz() { set_clock_speed(CLK_SPEED_2); }
void set_clock_4Hz() { set_clock_speed(CLK_SPEED_4); }
void set_clock_16Hz() { set_clock_speed(CLK_SPEED_16); }
void set_clock_32Hz() { set_clock_speed(CLK_SPEED_32); }
void set_clock_128Hz() { set_clock_speed(CLK_SPEED_128); }
void set_clock_256Hz() { set_clock_speed(CLK_SPEED_256); }

/* Toggle Arduino clock speed as long as the Arduino is in charge of it, ie.
 * it's not under control by the external clock. If we're manually clocking
 * the function will automatically switch to automatic mode using previously
 * used speed. 
 */
void do_toggle_speed() {
  if (clock_mode == CLK_MODE_MANUAL) {
      set_auto_clock();
  } else {
    int new_setting = clock_setting + 1;
    if (new_setting > CLK_MAX_SETTING) new_setting = 0;
    set_clock_speed(new_setting);
  }
}

void print_clock() {
  switch (clock_mode) {
    case CLK_MODE_NONE:
      Serial.print(F("Arduino clock is set to "));
      ansi_weak_ln(F("EXTERNAL"));
      break;
    case CLK_MODE_MANUAL:
      Serial.print(F("Arduino clock is set to "));
      ansi_weak_ln(F("MANUAL"));
      break;
    case CLK_MODE_AUTO:
      Serial.print(F("Arduino clock is set to "));
      ansi_weak(F("AUTO ("));
      Serial.print(delay_to_hz());
      Serial.print("Hz)");
      ansi_default();
      Serial.println();
      break;
  }
}

void set_monitor_off() {
  suppress_monitor = true;

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

  suppress_monitor = false;
}