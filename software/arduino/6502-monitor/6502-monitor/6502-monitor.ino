#include <Arduino.h>
#include "constants.h"
#include "commands.h"
#include "process_serial.h"
#include "process_switches.h"

bool colorize = true;
bool output = true;

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
  attachInterrupt(digitalPinToInterrupt(SBC_CLOCK), on_clock, RISING);
  
  print_welcome();
}

void loop() {
    while(Serial.available() > 0) {
        process_serial(Serial.read());
    }
    process_switches();
}
