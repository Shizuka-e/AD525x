/*
Sketchbook demonstrating the most basic functions of the AD5254 potentiometers, using the
AD525x.h library.

This logarithmically increases and decreases RDAC values using the built in commands.
*/

#include <Wire.h>
#include <AD525x.h>

AD5254 ad4;             // The potentiometer object, not initialized.
 
byte RDAC = 0x00;      // RDAC <= 3
byte AD_addr = 0b00;    // AD0 = 0, AD1 = 0
byte max_val = 0;

boolean going_up = true;

unsigned long sleep_time = 500;  // How frequently to change the resistance.

void setup() {
  // Initialize the AD5253 potentiometer with the AD address.
  ad4.initialize(AD_addr);
  ad4.reset_device();
  max_val = ad4.get_max_val();    // Maximum wiper value (resolution)
}

void loop() {
  // Increment the wiper value by one at each loop iteration.
   byte wiper_val = ad4.read_RDAC(RDAC);
   if(wiper_val == max_val) {
     going_up = false;
   } else if (wiper_val == 0) {
     going_up = true;
   }
   
   if (going_up) {
     ad4.increment_RDAC_6dB(RDAC);
   } else {
     ad4.decrement_RDAC_6dB(RDAC);
   }

  delay(sleep_time);
}
