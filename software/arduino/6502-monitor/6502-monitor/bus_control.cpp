#include <Arduino.h>
#include "constants.h"
#include "bus_control.h"
#include "debug.h"
#include "ansi.h"
#include "clock.h"

extern int clock_mode;
bool bus_asserted = false;

bool check_available() {
  switch (clock_mode) {
      case CLK_MODE_NONE:
        Serial.print(F("Arduino clock is set to "));
        ansi_error();
        Serial.print(F("EXTERNAL"));
        ansi_default();
        Serial.println();
        return false;
      case CLK_MODE_AUTO:
        do_auto_clock();
      case CLK_MODE_MANUAL:
        break; 
  }
  return true;
}

/* 
 * Assert control of address and data bus to enable direct control of the
 * various associated peripherals. Note that this assumes that we are able
 * to control the clock.
 */
void bus_assert() {
  if (bus_asserted) return;
  bus_asserted = true;

  debug(F("Assert READY"));
  digitalWrite(SBC_READY, LOW);
  pinMode(SBC_READY, OUTPUT);
  delay(10);
  debug(F("Assert BE"));
  digitalWrite(SBC_BE, LOW);
  pinMode(SBC_BE, OUTPUT);
  debug(F("Control R/W"));
  digitalWrite(SBC_RW, HIGH);
  pinMode(SBC_RW, OUTPUT);
  debug(F("Clock HIGH"));
  digitalWrite(SBC_CLOCK, HIGH);

  debug(F("Controlling A0-A15"));
  for (int n = 0; n < 16; n += 1) {
    pinMode(SBC_ADDR[n], OUTPUT);
    digitalWrite(SBC_ADDR[n], HIGH);
  }
}

/*
 * Release the bus, allowing the w65c02 to resume whatever it was doing before
 * we forced it offline. More than likely we've done so at a time where it
 * wasn't completely recoverable so you should probably be prepared to reset
 * it anyway!
 */
void bus_release() {
  if (!bus_asserted) return;
  debug(F("Release D0-D7"));
  for (int n = 0; n < 8; n += 1) {
    pinMode(SBC_DATA[n], INPUT);
  }

  debug(F("Release A0-A15"));
  for (int n = 0; n < 16; n += 1) {
    pinMode(SBC_ADDR[n], INPUT);
  }

  debug(F("Clock LOW"));
  digitalWrite(SBC_CLOCK, LOW);

  debug(F("Release R/W"));
  pinMode(SBC_RW, INPUT);

  debug(F("Deassert BE"));
  digitalWrite(SBC_BE, HIGH);
  pinMode(SBC_BE, OUTPUT);
  delay(10);
  debug(F("Deassert READY"));
  digitalWrite(SBC_READY, HIGH);
  pinMode(SBC_READY, INPUT);

  bus_asserted = false;
}

void set_data_direction(int direction) {
  digitalWrite(SBC_RW, HIGH);
  switch (direction) {
    case DATA_DIRECTION_READ:
      for (int i = 0; i < 8; i++) {
        pinMode(SBC_DATA[i], INPUT);
      }
      break;

    case DATA_DIRECTION_WRITE:
      for (int i = 0; i < 8; i++) {
        pinMode(SBC_DATA[i], OUTPUT);
      }
      break;
  }
}

void set_address(const unsigned int address) {
  unsigned int value = address;
  for (int i = 15; i >= 0; i--) {
    digitalWrite(SBC_ADDR[i], value & 1);
    value = value >> 1;
  }
}

/*
 * Read a byte from the data pins, use set_direction to perform sequential
 * reads without having to set up data direction every time (it's slow
 * enough as it is).
 */
byte read_byte(bool set_direction = true) {
  if (set_direction) set_data_direction(DATA_DIRECTION_READ);

  byte value = 0;
  for (int i = 0; i < 8; i++) {
    value = (value << 1) + digitalRead(SBC_DATA[i]);
  }
  return value;
}

/* 
 * Write a byte to the data pins, use set_direction to perform sequential
 * writes without having to set up data direction every time. Assumes that
 * the clock is already HI.
 */
void write_byte(byte value, bool set_direction = true) {
  if (set_direction) set_data_direction(DATA_DIRECTION_WRITE);

  for (int i = 7; i >= 0; i--) {
    digitalWrite(SBC_DATA[i], value & 1);
    value = value >> 1;
  }

  digitalWrite(SBC_RW, LOW);
  delayMicroseconds(1);
  digitalWrite(SBC_RW, HIGH);
}

/* 
 * Have a peek at the specified memory address, but will return a blank
 * value with all bits set if we were not able to access the bus.
 */
byte peek(const unsigned int address) {
  if (!check_available()) return 0xff;
  bus_assert();
  set_address(address);
  byte value = read_byte();
  bus_release();

  return value;
}

/*
 * Poke a byte into memory at the specified address, but not that no
 * checking will be done so make sure that the value returned is the same
 * you gave us in the first place!
 */
byte poke(const unsigned int address, byte value) {
  if (!check_available()) return 0xff;
  bus_assert();
  set_address(address);
  write_byte(value, true);
  value = read_byte();
  bus_release();
  return value;
}

/*
 * Dumps memory contents to the console, formatting it similarly to the hex
 * dumps found in the book "The First Book of KIM". Note that the code will
 * attempt to guess which blocks are blank and will only output the first of
 * them, then an asterisk will be printed in order to denote that a line has
 * been skipped.
 */
void dump(const unsigned int start_address, const unsigned int end_address) {
  if (!check_available()) return;
  bus_assert();

  bool last_blank = false;
  bool skipped = false;

  ansi_notice();
  Serial.println(F("        0  1  2  3  4  5  6  7    8  9  A  B  C  D  E  F "));
  ansi_default();

  set_data_direction(DATA_DIRECTION_READ);
  for (unsigned int base = start_address; base < end_address; base += 16) {
    byte data[16];
    int sum = 0;
    for (int offset = 0; offset <= 15; offset += 1) {
      set_address(base + offset);
      data[offset] = read_byte(false);
      sum += data[offset];
    }

    if (sum == 0 || sum == 4080) {
      if (!last_blank) {
        char buf[80];
        ansi_notice();
        sprintf(buf, "$%.4X- ", base);
        Serial.print(buf);
        ansi_default();

        sprintf(buf, "%02X %02X %02X %02X %02X %02X %02X %02X   %02X %02X %02X %02X %02X %02X %02X %02X",
                data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
                data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

        ansi_weak();
        Serial.println(buf);
        ansi_default();
        last_blank = true;
      } else {
        if (!skipped) {
          ansi_colour(COLOUR_WHITE);
          Serial.println("       *");
          ansi_default();
          skipped = true;
        }
      }
    } else {
      skipped = false;
      last_blank = false;

      char buf[80];
      ansi_notice();
      sprintf(buf, "$%.4X- ", base);
      Serial.print(buf);
      ansi_default();

      sprintf(buf, "%02X %02X %02X %02X %02X %02X %02X %02X   %02X %02X %02X %02X %02X %02X %02X %02X",
              data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
              data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

      Serial.println(buf);
    }

    /* Catch unsigned integer roll-over */
    if ((base + 16) == 0) break;
  }

  bus_release();
}

void dump_ram() {
  dump(0x0000, 0x3fff);
}

void dump_zero_page() {
  dump(0x0000, 0x00ff);
}

void dump_stack() {
  dump(0x0100, 0x01ff);
}

void dump_rom() {
  dump(0x8000, 0xffff);
}

void dump_vectors() {
  dump(0xfff0, 0xffff);
}

void do_bus_test() {
  if (!check_available()) return;

  bus_assert();
  for (int i = 0; i < 0xff; i++) {
    set_address(0x0100 + i);
    write_byte(0xff);
  }
  bus_release();

  dump_stack();
}