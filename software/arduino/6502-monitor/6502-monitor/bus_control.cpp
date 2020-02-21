#include <Arduino.h>
#include "constants.h"
#include "bus_control.h"
#include "debug.h"
#include "ansi.h"
#include "clock.h"
#include "commands.h"

extern int clock_mode;

extern int command_set;
bool bus_asserted = false;

/*
 * Set the direction of the pins currently facing the computers data bus, this
 * needs to be done so that it corresponds to whatever signalling we are
 * performing. Note that incorrect use will lead to a dead Arduino, SBC or
 * both!
 */
void set_data_direction(int direction) {
  digitalWrite(SBC_RW, HIGH);
  switch (direction) {
    case DATA_DIRECTION_READ:
      debug(F("Set data direction: READ"));
      for (int i = 0; i < 8; i++) {
        pinMode(SBC_DATA[i], INPUT);
      }
      break;

    case DATA_DIRECTION_WRITE:
      debug(F("Set data direction: WRITE"));
      for (int i = 0; i < 8; i++) {
        pinMode(SBC_DATA[i], OUTPUT);
      }
      break;
  }
}

/* 
 * Assert control of address and data bus to enable direct control of the
 * various associated peripherals. Note that this assumes that we are able
 * to control the clock.
 */
void bus_assert() {
  if (bus_asserted) return;
  bus_asserted = true;

  int_detach();

  debug(F("Assert READY"));
  digitalWrite(SBC_READY, LOW);
  pinMode(SBC_READY, OUTPUT);

  debug(F("Clock HIGH"));
  digitalWrite(SBC_CLOCK, HIGH);

  debug(F("Assert BE"));
  digitalWrite(SBC_BE, LOW);
  pinMode(SBC_BE, OUTPUT);

  debug(F("Control R/W"));  
  digitalWrite(SBC_RW, HIGH);
  pinMode(SBC_RW, OUTPUT);

  debug(F("Controlling A0-A15"));
  for (int n = 0; n < 16; n += 1) {
    pinMode(SBC_ADDR[n], OUTPUT);
    digitalWrite(SBC_ADDR[n], HIGH);
  }

  set_data_direction(DATA_DIRECTION_READ);
}

/*
 * Release the bus, allowing the w65c02 to resume whatever it was doing before
 * we forced it offline. More than likely we've done so at a time where it
 * wasn't completely recoverable so you should probably be prepared to reset
 * it anyway!
 */
void bus_release() {
  if (!bus_asserted) return;
  set_data_direction(DATA_DIRECTION_READ);

  debug(F("Release A0-A15"));
  for (int n = 0; n < 16; n += 1) {
    pinMode(SBC_ADDR[n], INPUT);
  }

  debug(F("Release R/W"));
  pinMode(SBC_RW, INPUT);

  debug(F("Deassert BE"));
  digitalWrite(SBC_BE, HIGH);
  pinMode(SBC_BE, OUTPUT);

  debug(F("Clock LOW"));
  digitalWrite(SBC_CLOCK, LOW);

  debug(F("Deassert READY"));
  digitalWrite(SBC_READY, HIGH);
  pinMode(SBC_READY, INPUT);

  int_attach();

  bus_asserted = false;
}

/*
 * Set address pins, unsigned to allow for the full 65K address space.
 */
void set_address(const unsigned long address) {
  unsigned long value = address;
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
byte peek(const unsigned long address) {
  set_address(address);
  return read_byte();
}

/*
 * Poke a byte into memory at the specified address, but not that no
 * checking will be done so make sure that the value returned is the same
 * you gave us in the first place!
 */
byte poke(const unsigned long address, byte value) {
  set_address(address);
  write_byte(value, true);
  return read_byte();
}

void zero_memory(const unsigned int start_address, const unsigned int end_address, byte value = 0x00) {
  unsigned int num_bytes = 0;
  unsigned int base = start_address;
  set_data_direction(DATA_DIRECTION_WRITE);
  while (base <= end_address) {
    set_address(base);
    write_byte(value, false);
    base++;
    num_bytes++;
  }
  set_data_direction(DATA_DIRECTION_READ);
  ansi_notice();
  Serial.print(num_bytes);
  Serial.println(F(" bytes written."));
  ansi_default();
}
void zero_zp() { zero_memory(0x0000, 0x00ff); }
void zero_stack() { zero_memory(0x0100, 0x01ff); }
void zero_ram() { zero_memory(0x0000, 0x3fff); }

/*
 * Dumps memory contents to the console, formatting it similarly to the hex
 * dumps found in the book "The First Book of KIM". Note that the code will
 * attempt to guess which blocks are blank and will only output the first of
 * them, then an asterisk will be printed in order to denote that a line has
 * been skipped.
 */
void dump_memory(const unsigned long start_address, const unsigned long end_address) {
  bool last_blank = false;
  bool skipped = false;
  set_data_direction(DATA_DIRECTION_READ);

  ansi_notice_ln(F("        0  1  2  3  4  5  6  7    8  9  A  B  C  D  E  F "));
  for (unsigned long base = start_address; base < end_address; base += 16) {
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
        sprintf(buf, "$%.4X- ", (int)base);
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
      sprintf(buf, "$%.4X- ", (unsigned int)base);
      Serial.print(buf);
      ansi_default();

      sprintf(buf, "%02X %02X %02X %02X %02X %02X %02X %02X   %02X %02X %02X %02X %02X %02X %02X %02X",
              data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
              data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

      Serial.println(buf);
    }
  }

  ansi_notice_ln(F("done."));
}
void dump_ram() { dump_memory(0x0000, 0x3fff); }
void dump_ram_1k() { dump_memory(0x0000, 0x03ff); }
void dump_ram_2k() { dump_memory(0x0000, 0x07ff); }
void dump_ram_4k() { dump_memory(0x0000, 0x0fff); }
void dump_ram_8k() { dump_memory(0x0000, 0x1fff); }
void dump_ram_16k() { dump_ram(); }
void dump_zp() { dump_memory(0x0000, 0x00ff); }
void dump_stack() { dump_memory(0x0100, 0x01ff); }
void dump_rom() { dump_memory(0x8000, 0xffff); }
void dump_vectors() { dump_memory(0xfff0, 0xffff); }

int intel_checksum(int byte_count, int hi, int lo, int record_type, int data_sum) {
  int x = byte_count + hi + lo + record_type + data_sum;
  x = x % 256;
  x = ~x;
  x = x + 1;
  x = x & 0xFF;
  return x;
}

/*
 * Handle exporting of data to Intel HEX format, this data is printed to
 * serial directly with a 16 byte record length along with the expected
 * checksum. Note that addresses specified are in relation to the system!
 */
void dump_intel(const unsigned long start_address, const unsigned long end_address) {
  set_data_direction(DATA_DIRECTION_READ);
  int byte_count = 16;
  
  for (unsigned long base = start_address; base < end_address; base += 16) {
    byte data[byte_count];
    int hi = (base & 0xFF00) >> 8;
    int lo = base & 0x00FF;

    int sum = 0;
    for (int offset = 0; offset < byte_count; offset += 1) {
      set_address(base + offset);
      data[offset] = read_byte(false);
      sum += data[offset];
    }

    int checksum = intel_checksum(
      0x10, 
      hi,
      lo,
      0x00,
      sum
    );

    char buf[80];
    sprintf(buf, ":%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
            byte_count, hi, lo, 0x00,
            data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], checksum);

    Serial.println(buf);
  }

  ansi_notice_ln(F("done."));
}
void dump_intel_ram() { dump_intel(0x0000, 0x3fff); }
void dump_intel_ram_1k() { dump_intel(0x0000, 0x03ff); }
void dump_intel_ram_2k() { dump_intel(0x0000, 0x07ff); }
void dump_intel_ram_4k() { dump_intel(0x0000, 0x0fff); }
void dump_intel_ram_8k() { dump_intel(0x0000, 0x1fff); }
void dump_intel_ram_16k() { dump_intel_ram(); }
void dump_intel_rom() { dump_intel(0x8000, 0xffff); }
void dump_intel_rom_1k() { dump_intel(0x8000, 0x83ff); }
void dump_intel_rom_2k() { dump_intel(0x8000, 0x87ff); }
void dump_intel_rom_4k() { dump_intel(0x8000, 0x8fff); }
void dump_intel_rom_8k() { dump_intel(0x8000, 0x9fff); }
void dump_intel_rom_16k() { dump_intel(0x8000, 0xbfff); }
void dump_intel_rom_32k() { dump_intel_rom(); }
void dump_intel_stack() { dump_intel(0x0100, 0x01ff); }
void dump_intel_zp() { dump_intel(0x0000, 0x00ff); }

/*
 * Tests a supplied segment of memory by writing the specified pattern to the
 * entire block specified, then attempt to read it back to ensure that we are
 * able to retrieve the same value.
 */
bool test_memory_pattern(const unsigned long start_address, const unsigned long end_address, const byte pattern) {
  unsigned long base = start_address;
  bool passed = true;
  #ifdef DEBUG
  char tmp[20];
  #endif

  bool done = false;
  while (!done) {
    if (end_address < base) break;
    int max_bytes = (end_address + 1) - base;
    if (MEMORY_BLOCK_SIZE < max_bytes) max_bytes = MEMORY_BLOCK_SIZE;

    #ifdef DEBUG
    sprintf(tmp, "0x%04X (%d bytes)", (unsigned int)base, max_bytes);
    ansi_debug_ln(tmp);
    #endif

    set_data_direction(DATA_DIRECTION_WRITE);
    for (int i = 0; i < max_bytes; i++) {
      set_address(base + i);
      write_byte(pattern, false);
    }

    set_data_direction(DATA_DIRECTION_READ);
    for (int i = 0; i < max_bytes; i++) {
      set_address(base + i);
      if (pattern != read_byte(false)) {
        passed = false;
        break;
      }
    }

    base += max_bytes;
    if (!passed) break;
  }

  set_data_direction(DATA_DIRECTION_READ);
  return passed;
}

/*
 * Perform a test of the configured amount of memory, this section of memory
 * has different patterns written to it in 16 byte blocks before comparing
 * the value to what is read back.
 */
void test_memory(const unsigned long start_address, const unsigned long end_address) {
  const byte patterns[] = {0x55, 0xaa, 0x00, 0xff};

  for (int i = 0; i < 4; i++) {
    char tmp[10];
    ansi_notice(F("Pass "), false);
    sprintf(tmp, "%d (0x%02X) ", i, patterns[i]);
    Serial.println(tmp);
    ansi_default();

    if (test_memory_pattern(start_address, end_address, patterns[i])) {
      ansi_notice_ln(F(" OK!"));
    } else {
      ansi_error_ln(F(" failed!"));
      break;
    }
  }
  
  ansi_notice_ln(F("Memory test completed."));
}
void test_ram() { test_memory(0x000, 0x3fff); }
void test_zp() { test_memory(0x0000, 0x00ff); }
void test_stack() { test_memory(0x0100, 0x01ff); }

/*
 * Check that we are able to transition into bus control mode, since we could
 * just as well be connected to an external clock module we'll require that
 * the user has previously unlocked the Arduino clock (held down switch 2 or 3,
 * or entered the corresponding serial command).
 */
bool check_control_dependencies() {
  switch (clock_mode) {
      case CLK_MODE_NONE:
        return false;
      case CLK_MODE_AUTO:
        set_manual_clock();
      case CLK_MODE_MANUAL:
        break; 
  }
  return true;
}

/*
 * Transition into bus control mode, but only if pre-flight checks are
 * successful (see check_available). Note that this will redefine the user
 * interface to match the new mode (both switches and serial commands).
 */
void set_control_on() {
  if (!check_control_dependencies()) {
    ansi_error_ln(F("Arduino clock is set to EXTERNAL"));
    return;
  }
  command_set = COMMAND_SET_CONTROL;
  bus_assert();

  Serial.print(F("BUS Control mode "));
  ansi_highlight_ln(F("ENABLED"));
}

/*
 * Transition out of bus control mode, resuming normal operation.
 */
void set_control_off() {
  bus_release();
  command_set = COMMAND_SET_MAIN;

  Serial.print(F("BUS Control mode "));
  ansi_highlight_ln(F("DISABLED"));
}