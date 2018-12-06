#include <SPI.h>

#include "RF24.h"//External library, Source: https://github.com/nRF24/RF24

//These are separate class files (PLEASE MARK THIS TOO)
#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/Communications.h"
#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/LedControl.h"
#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/Commands.h"
RF24 radio(7,8);
Communication COMMS(true);
Led signalIndicatorLED(4);
Led noSignalLED(9);

//LED push buttons
int podLastTrigger =0;
int detatchButtonPin = 3;
Led detatchButtonLED(5);

int parachuteLastTrigger =0;
int parachuteButtonPin = 2;
Led parachuteButtonLED(6);

boolean thereIsSignal = true;
void setup() {
  
  Serial.begin(9600);
  COMMS.waitForRadioModule(radio); //Waits for radio module to be initialised

   attachInterrupt(digitalPinToInterrupt(detatchButtonPin), detatchPodButtonTrigger, CHANGE);
   attachInterrupt(digitalPinToInterrupt(parachuteButtonPin), detatchParachuteButtonTrigger, CHANGE);
  parachuteButtonLED.on();
  signalIndicatorLED.on();
  detatchButtonLED.on();
}

// the loop function runs over and over again forever
void loop() {
  //Lets update the LED states
  progressLEDEpochs();
  setSignalIndicator(COMMS.ping(100, radio));
  
  delay(100);

}

void progressLEDEpochs(){
  signalIndicatorLED.progressEpoch();
  parachuteButtonLED.progressEpoch();
  detatchButtonLED.progressEpoch();
}

void setSignalIndicator(boolean isSignal){
 if(thereIsSignal != isSignal){
  thereIsSignal = isSignal;
  if(isSignal){
    Serial.println("SIGNAL Regained");
    signalIndicatorLED.on();
  }
  else{
    Serial.println("[WARNING] - SIGNAL LOST");
    signalIndicatorLED.flash(400);
  }
 }
}

void transmitSetCommand(float command){
  COMMS.writeFloat(command, radio);
}

void detatchParachuteButtonTrigger(){
  
  if((millis()-parachuteLastTrigger)>500){
    transmitSetCommand(DEPLOY_PARACHUTE);
  parachuteButtonLED.flash(1000);
  }

  parachuteLastTrigger = millis();
}
void detatchPodButtonTrigger(){
  if((millis()-podLastTrigger)>500){
  detatchButtonLED.flash(1000);
  transmitSetCommand(DETATCH_POD);
  }
  podLastTrigger = millis();
}



