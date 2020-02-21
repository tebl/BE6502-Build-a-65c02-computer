#include <Arduino.h>
#include <TimerThree.h>
#include "constants.h"
#include "commands.h"
#include "process_serial.h"
#include "process_switches.h"

/* Able to use VT100 commands on serial, disable if you get wierd characters
 * in serial output - or switch to a serial terminal that supports them.
 */
bool ansi_enabled = true;     

/* Enable bus monitor output */
volatile bool suppress_monitor = false;
bool int_enabled = false;

/* Controls the ability of the Arduino Mega 2560 to generate the system clock,
 * but since we don't want two clocks fighting eachother this is normally
 * disabled until explicitly enabled.
 */
int clock_mode = CLK_MODE_NONE;
int clock_setting = 0;

void setup() {
  Serial.begin(115200);
  for (int n = 0; n < 16; n += 1) {
    pinMode(SBC_ADDR[n], INPUT);
  }
  for (int n = 0; n < 8; n += 1) {
    pinMode(SBC_DATA[n], INPUT);
  }

  pinMode(USER_LED, OUTPUT);
  pinMode(USER_SW1, INPUT);
  pinMode(USER_SW2, INPUT);
  pinMode(USER_SW3, INPUT);

  pinMode(SBC_CLOCK, INPUT);
  pinMode(SBC_RW, INPUT);
  pinMode(SBC_BE, INPUT);
  pinMode(SBC_READY, INPUT);
  
  commands_init();
  print_welcome();
}

void loop() {
    while(Serial.available() > 0) {
        process_serial(Serial.read());
    }
    process_switches();
}
