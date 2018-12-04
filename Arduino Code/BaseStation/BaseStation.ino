#include <SPI.h>
#include "RF24.h"

#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/Communications.h"
#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/LedControl.h"
RF24 radio(7,8);
Communication COMMS(true);

Led indicatorLight(LED_BUILTIN);
void setup() {
  
  Serial.begin(9600);
  COMMS.waitForRadioModule(radio); //Waits for radio module to be initialised
  indicatorLight.flash(5000);
  digitalWrite(LED_BUILTIN, HIGH);
}

// the loop function runs over and over again forever
void loop() {
  //indicatorLight.progressEpoch();
  //Serial.println("Progressing");
}
