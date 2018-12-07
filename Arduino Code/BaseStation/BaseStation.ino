//Arduino Libraries
#include <SPI.h>

//Externally Sourced Libraries
#include "RF24.h"//External library, Source: https://github.com/nRF24/RF24

//These are separate class files (PLEASE MARK THIS TOO)
#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/Communications.h"
#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/LedControl.h"
#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/Commands.h"
RF24 radio(7,8);
Communication COMMS(true);
Led commandFailLED(9);
Led signalIndicatorLED(4);

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

  //Interrupts stops the processor and executes a specific method
   attachInterrupt(digitalPinToInterrupt(detatchButtonPin), detatchPodButtonTrigger, RISING);
   attachInterrupt(digitalPinToInterrupt(parachuteButtonPin), detatchParachuteButtonTrigger,RISING);

  //Sets all of the LEDs to default states
  parachuteButtonLED.on();
  signalIndicatorLED.on();
  detatchButtonLED.on();
  commandFailLED.off();
}

// Where all the stuff happens
void loop() {
  //Lets update the LED states
  progressLEDEpochs();

  boolean signalI = COMMS.ping(100, radio); //Is the drop pod alive and well?
  setSignalIndicator(signalI); //Lets update the signal LED to reflect that
  printOrientation();
  delay(100);//Wait makes sure drop pod isn't bombarded with requests
  
}

void printOrientation(){
  int startTime = millis();
  boolean timeout= false;
  float orientation =0;
  int successful =1;
  if(thereIsSignal){
    
    COMMS.writeFloat(GET_ORIENTATION, radio);
    
    while(!COMMS.incoming(radio) && !(timeout = ((millis()-startTime)>100)));
      
    if(!timeout){//Did we timeout?
    //No
      orientation = COMMS.read(1, radio)[0];
    }
    else{
    //Yes
      successful =0;
    }
  }
    else{
     successful = 0;
    }
    Serial.println(orientation);
    Serial.write((int)orientation);
    Serial.write(',');
    Serial.write(successful);
    Serial.write(';');
  
}

void progressLEDEpochs(){
  commandFailLED.progressEpoch();
  signalIndicatorLED.progressEpoch();
  parachuteButtonLED.progressEpoch();
  detatchButtonLED.progressEpoch();
}

void setSignalIndicator(boolean isSignal){
 if(thereIsSignal != isSignal){
  thereIsSignal = isSignal;
  if(isSignal){
   // Serial.println("SIGNAL Regained");
    signalIndicatorLED.on();
  }
  else{
    //Serial.println("[WARNING] - SIGNAL LOST");
    signalIndicatorLED.flash(400);
  }
 }
}

void transmitSetCommand(float command){
  COMMS.writeFloat(command, radio);
}

void detatchParachuteButtonTrigger(){
  
  if((millis()-parachuteLastTrigger)>500){
     if(!thereIsSignal) commandFailLED.flash(200);
  else commandFailLED.off();
    transmitSetCommand(DEPLOY_PARACHUTE);
  parachuteButtonLED.flash(1000);
  }

  parachuteLastTrigger = millis();
}
void detatchPodButtonTrigger(){
  if((millis()-podLastTrigger)>500){
  detatchButtonLED.flash(1000);
  if(!thereIsSignal) commandFailLED.flash(200);
  else commandFailLED.off();
  transmitSetCommand(DETATCH_POD);
  }
  podLastTrigger = millis();
}




