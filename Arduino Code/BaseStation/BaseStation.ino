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
int podLastTrigger =0; //last time the button was triggered in millis
int detatchButtonPin = 3; // This is the digital pin for that button
Led detatchButtonLED(5); // The LED class associated with this button

int parachuteLastTrigger =0;
int parachuteButtonPin = 2;
Led parachuteButtonLED(6);

boolean thereIsSignal = true; // Specifies whether there is signal
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

//
void printOrientation(){
  float startTime = millis();
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

    Serial.println(""+String(orientation)+","+successful+";");
   
  
}

//For each LED, the pogressEpoch method calculate at what point the LED is current
//in (on or off) and set the LED to that state, this is because the arduino does not
//carry any multi-threadin support, so we have made our own
void progressLEDEpochs(){
  commandFailLED.progressEpoch();
  signalIndicatorLED.progressEpoch();
  parachuteButtonLED.progressEpoch();
  detatchButtonLED.progressEpoch();
}

//This method sets the signal LED to reflect whether there is a signal
//between the base station and the drop pod
void setSignalIndicator(boolean isSignal){
 if(thereIsSignal != isSignal){ // Makes sure same state is not updated twice
  thereIsSignal = isSignal;
  if(isSignal){
   // Serial.println("SIGNAL Regained");
    signalIndicatorLED.on(); // Solid LED show a constant signal is being held
  }
  else{
    //Serial.println("[WARNING] - SIGNAL LOST");
    signalIndicatorLED.flash(400); // Flashes to indicate signal lost
  }
 }
}

void transmitSetCommand(float command){
  COMMS.writeFloat(command, radio);
}

//When the detatch button is pressed, this method is called.
void detatchParachuteButtonTrigger(){
  
  if((millis()-parachuteLastTrigger)>500){ // Has it been at least 500 milliseconds since last trigger?
     if(!thereIsSignal) commandFailLED.flash(200); //Make sure there is signal before executing the command
  else commandFailLED.off();
    transmitSetCommand(DEPLOY_PARACHUTE); // Sends signal to deploy parachute
  parachuteButtonLED.flash(1000); //Starts LED flash to show completion of operation
  }

  parachuteLastTrigger = millis(); // This counter acts the effect of button bouncing
}

//Similar structure to the method: detatchParachuteButtonTrigger()
//However, here the command is transmitted to detatch the pod
void detatchPodButtonTrigger(){
  if((millis()-podLastTrigger)>500){
  detatchButtonLED.flash(1000);
  if(!thereIsSignal) commandFailLED.flash(200);
  else commandFailLED.off();
  transmitSetCommand(DETATCH_POD);
  }
  podLastTrigger = millis();
}




