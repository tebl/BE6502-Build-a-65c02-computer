#include <Arduino.h>
#include "6502-monitor.h"
#include "constants.h"
#include "commands.h"
#include "process_serial.h"

bool colorize = true;
bool output = true;

void setup() {
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
  attachInterrupt(digitalPinToInterrupt(SBC_CLOCK), onClock, RISING);
  
  Serial.begin(115200);
  print_welcome();
}

void onClock() {
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

void loop() {
    while(Serial.available() > 0) {
        process_serial(Serial.read());
    }
}
