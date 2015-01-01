/*
Sketchbook demonstrating the most basic functions of the AD5254 potentiometers, using the
AD525x.h library.

This linearly varies the resistance of all RDACs on an AD5254 from 0 to the maximum, then back again.
*/

#include <Wire.h>
#include <AD525x.h>

AD5254 ad4;             // The potentiometer object, not initialized.

byte AD_addr = 0b00;    // AD0 = 0, AD1 = 0
int16_t wiper_val = 0;     // Start the wiper value at 0.
uint8_t increment_val = 1;  // Wiper increment
int8_t inc_direction = 1;
byte max_val = 0;

unsigned long sleep_time = 100;  // How frequently to change the resistance.

void setup() {
  // Initialize the AD5253 potentiometer with the AD address.
  ad4.initialize(AD_addr);
  max_val = ad4.get_max_val();    // Maximum wiper value (resolution)
}

void loop() {
  // Increment the wiper value by one at each loop iteration.
  for(byte RDAC = 0; RDAC < 4; RDAC++) {    
     ad4.write_RDAC(RDAC, wiper_val);
  }
  
  wiper_val += increment_val * inc_direction;
    
  if (wiper_val >= max_val) {
    wiper_val = max_val;
    inc_direction = -1;
  } else if (wiper_val <= 0) {
    wiper_val = 0;
    inc_direction = +1;
  }

  delay(sleep_time);
}
