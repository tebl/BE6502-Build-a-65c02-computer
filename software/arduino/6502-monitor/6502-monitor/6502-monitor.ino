#include <Arduino.h>
#include "6502-monitor.h"
#include "constants.h"
#include "commands.h"
#include "process_serial.h"

bool colorize = true;

void setup() {
  for (int n = 0; n < 16; n += 1) {
    pinMode(ADDR[n], INPUT);
  }
  for (int n = 0; n < 8; n += 1) {
    pinMode(DATA[n], INPUT);
  }
  pinMode(CLOCK, INPUT);
  pinMode(READ_WRITE, INPUT);

  //attachInterrupt(digitalPinToInterrupt(PHI2), onClock, RISING);
  
  Serial.begin(115200);
  print_welcome();
}

void onClock() {
  char output[15];

  unsigned int address = 0;
  for (int n = 0; n < 16; n += 1) {
    int bit = digitalRead(ADDR[n]) ? 1 : 0;
    Serial.print(bit);
    address = (address << 1) + bit;
  }
  
  Serial.print("   ");
  
  unsigned int data = 0;
  for (int n = 0; n < 8; n += 1) {
    int bit = digitalRead(DATA[n]) ? 1 : 0;
    Serial.print(bit);
    data = (data << 1) + bit;
  }

  sprintf(output, "   %04X  %c %02X", address, digitalRead(READ_WRITE) ? 'r' : 'W', data);
  Serial.println(output);  
}

void loop() {
    while(Serial.available() > 0) {
        process_serial(Serial.read());
    }
}
