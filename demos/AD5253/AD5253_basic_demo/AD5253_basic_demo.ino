/*
Sketchbook demonstrating the most basic functions of the AD5253 potentiometers, using the
AD525x.h library.

This linearly varies the resistance of a specified RDAC on the specified device from 0 to the maximum.
*/

#include <Wire.h>
#include <AD525x.h>

AD5253 ad3;             // The potentiometer object, not initialized.

byte AD_addr = 0b00;    // AD0 = 0, AD1 = 0
byte RDAC = 0;          // RDAC can take values [0, 3]

byte wiper_val = 0;     // Start the wiper value at 0.
byte max_val = 0;

unsigned long sleep_time = 100;  // How frequently to change the resistance.

void setup() {
  // Initialize the AD5253 potentiometer with the AD address.
  ad3.initialize(AD_addr);
  
  max_val = ad3.get_max_val();    // Maximum wiper value (resolution)
}

void loop() {
  // Increment the wiper value by one at each loop iteration.
  ad3.write_RDAC(RDAC, wiper_val++);
  if (wiper_val > max_val) {
    wiper_val = 0;      // Restart if we get to the end.
  }
  
  delay(sleep_time);
}
