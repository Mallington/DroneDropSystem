//Arduino Libraries
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>

//Externally Sourced Libraries
#include <MPU6050.h> //External library, Source: https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050
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

//Accelerometer values and objects
float pitch = 0; //current pitch
float roll = 0;//current roll
float yaw = 0;//current yaw
float lastUpdateTime =0;//last time in millis that the roll,pitch,yaw was updated
MPU6050 mpu; //Initiation of MPU object

void setup() {
  Serial.begin(9600);
  
  mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G);//Initialises the module
  mpu.calibrateGyro();//Sets default positions to 0
  mpu.setThreshold(3);//Sets the threshold value
  
  //Tells the class which pin to communicate with when controlling the servo
  parachuteCatch.attach(parachuteCatchPin); 
  podCatch.attach(podCatchPin);

  //Sets both the pod catch mechanism and the parachute mechanism to the close position
   podCatch.write(POD_CATCH_CLOSE);
   parachuteCatch.write(PARACHUTE_CATCH_CLOSE);
  
  
  COMMS.waitForRadioModule(radio); //Waits for radio module to be initialised before proceeding
  COMMS.setListening(true, radio); //Starts to actively listens for incoming packets
}

/*The drop pod acts as a sort of REST server, it only acts upon commands that are recieved
*such as data requests (eg. GET_ACCELERATION) or commands (eg. POD_CATCH_OPEN)
*
*/
void loop() {
  checkForIncomingData();//Let's see if there is any incoming data right now
  updateGyroMovement();//Lets update our current orientation
}

//Checks and acts upon any incoming data packets
void checkForIncomingData(){
   if(COMMS.incoming(radio)){ //Are there incoming data packets?
    //There is
    parseIncoming(COMMS.read(1, radio)[0]); // Lets read them to an array and parse the command we just captured
  }
}

//Updates 
void updateGyroMovement(){
   // Read normalized values
  Vector norm = mpu.readNormalizeGyro();
  
  float lastUpdatePeriod = ((millis())-(lastUpdateTime))/1000.0; //Calculates time in seconds since last gyro movement update
 
  lastUpdateTime= millis(); //Sets time period for next calculation

  // Calculate Pitch, Roll and Yaw
  pitch = pitch + norm.YAxis * lastUpdatePeriod;
  roll = roll + norm.XAxis * lastUpdatePeriod;
  yaw = yaw + norm.ZAxis * lastUpdatePeriod;
}

void parseIncoming(float arr){
//For all command definitions, see Communications.h file for comments
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

  if(arr == GET_ORIENTATION){
    COMMS.writeFloat(pitch,radio);
  }
  
}
