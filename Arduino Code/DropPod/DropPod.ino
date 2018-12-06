//Library 
#include <Servo.h>
#include <SPI.h>

#include "RF24.h"//External library, Source: https://github.com/nRF24/RF24

//These are separate class files (PLEASE MARK THIS TOO)
#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/Communications.h"
#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/LedControl.h"
#include "/Users/mathew/Documents/GitHub/DroneDropSystem/Arduino Code/Custom Libraries/Commands.h"

RF24 radio(7,8);//External library object initiation, used for a direct interface with the RF24 module
Communication COMMS(false); //Communication object initiation, used for data transmission

//Values contains the specific servo positions for a specific state.
//POD Catch Servo positions
int POD_CATCH_CLOSE = 45; //Servo mechanism attatching pod to drone is closed
int POD_CATCH_OPEN = 130;
//Parachute catch servo postions
int PARACHUTE_CATCH_CLOSE = 60; //Servo mechanism securing parachute to pod is closed
int PARACHUTE_CATCH_OPEN = 160;

//Servo mechanisms in the pod
Servo parachuteCatch; //This mechanism holds the parachute in when closed, when open the parachute is deployed via elastic potential
int parachuteCatchPin = 9; //Pin value referenced with the signal wire of this servo
Servo podCatch; //This mechanism traps the latch on the pod into a slot in the drone harness, securing it too the drone
int podCatchPin = 10; 

void setup() {
  Serial.begin(9600);

  parachuteCatch.attach(parachuteCatchPin);
  podCatch.attach(podCatchPin);

   podCatch.write(POD_CATCH_CLOSE);
   parachuteCatch.write(PARACHUTE_CATCH_CLOSE);
  
  
  COMMS.waitForRadioModule(radio); //Waits for radio module to be initialised
  COMMS.setListening(true, radio); //Actively listens for incoming packets
}

void loop() {
  checkForIncomingData();
   
}

//Checks and acts upon any incoming data packets
void checkForIncomingData(){
   if(COMMS.incoming(radio)){ //Are there incoming data packets?
    //There is
    parseIncoming(COMMS.read(1, radio)[0]); // Lets read them to an array and parse the command we just captured
  }
}
void parseIncoming(float arr){
  Serial.println("Packet recieved:"+String(arr));
  if(arr == PING){
    COMMS.pong(arr, radio);
  }
  if(arr == DETATCH_POD){
    podCatch.write(POD_CATCH_OPEN);
  }
  if(arr == ATTATCH_POD){
    podCatch.write(POD_CATCH_CLOSE);
  }

  if(arr == DEPLOY_PARACHUTE){
    parachuteCatch.write(PARACHUTE_CATCH_OPEN);
  }

  if(arr == CLOSE_PARACHUTE_CATCH){
    parachuteCatch.write(PARACHUTE_CATCH_CLOSE);
  }

  if(arr == GET_ACCELERATION){
    
  }

  if(arr == GET_ORIENTATION){
    
  }
  
}
