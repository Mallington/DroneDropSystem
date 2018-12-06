/*
* This class was created so that an LED could flash at a set rate without locking up the single thread of the arduino
* So instead of using a conventional loop, the progressEpoch method turns that specific pin on or off depending or what point in the flash cycle it is at
*So progressEpoch() is called in main loop only when there is chance to, giving other components such as the RF24 module higher priority
*/
class Led{
private:
  int LED;
  boolean flashing = false; // States whether the LED is in flashing mode
  boolean lastHigh = false; // states whether the LED has already been set to high
  int startTime =0;//This is the time in millis when the LED flash cycle was commenced
  int period =0;// This is the period of time for which an LED is turned on and off for
  int lastEpochProg =-1;// This is the last time period at which the LED state was last updated
public:
  Led(int pin);
  void on();
  void off();
  void flash(int timePeriod);
  void progressEpoch();
};



Led::Led(int pin)
{
	pinMode(pin, OUTPUT);
  LED = pin;
}

void Led::on(){ // sets the LED to high and stops any flashing that might currently be occuring
  flashing = false; // lets stop flashing
  digitalWrite(LED, HIGH); //Sets the LED to high / ON
}
void Led::off(){ // // sets the LED to low and stops any flashing that might currently be occuring
  flashing = false;
  digitalWrite(LED, LOW); //Sets the LED to low / OFF
}

//This is used to update an LED state by calculating at what point the LED should be in the cycle using the millis() which is used to show how much time is elapsed
void Led::progressEpoch(){
  if(flashing){ // Is this LED currently in flashing mode?
    // Yes it is
    int epochProg = (millis()-startTime)/(period/2); // Calculates how many half epochs/ half time periods has progressed since the flash() method was called
    if(lastEpochProg != epochProg) { //Has this current epoch already updated its state
      //Nope hasn't updated this epoch yet
      lastEpochProg = epochProg;
      if(epochProg %2 ==0){ // Is the half epoch number even?
        //Yes
        digitalWrite(LED, HIGH); //Lets set the LED to high
        Serial.println("High:"+String(LED));
      }
      else{
        //No it is odd
        Serial.println("Low: "+String(LED)); 
        digitalWrite(LED, LOW); // Lets set it to low
      }
  }
  }
}

//Sets an LED to flash at a certain frequency determined by (1/timePeriod)
void Led::flash(int timePeriod){
  period = timePeriod;// Time period at which the LED should turn on and off (once complete cycle)
  startTime = millis(); // records the start point, will become useful when calculating at what point in the flash cycle the LED is at later
  flashing = true; //Yep, lets start flashing
}



