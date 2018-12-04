byte POD[6] = "1Node";
byte TRANSMITTER[6] = "2Node";
class Communication {
private:
  boolean radioModuleUp = false;
  boolean isBaseStation = false;
public:
  boolean setupComms(RF24 radio);
  boolean isModuleUp();
  Communication(boolean thisIsBaseStation);
  void waitForRadioModule(RF24 Module);
};

// radio.startListening(); , radio.stopListening();       

boolean Communication::setupComms(RF24 radio){
  radioModuleUp = radio.begin();
//  radio = new RF24(7,8);
if(radioModuleUp){
  if(isBaseStation){
    radio.openWritingPipe(POD);
    radio.openReadingPipe(1,TRANSMITTER);
  }
  else{
    radio.openWritingPipe(TRANSMITTER);
    radio.openReadingPipe(1,POD);
  }

  radio.setPALevel(RF24_PA_LOW); // Need to experiment
}
  return radioModuleUp;
}

boolean Communication :: isModuleUp(){
  return radioModuleUp;
}

Communication::Communication(boolean thisIsBaseStation)
{
	
	isBaseStation = thisIsBaseStation;
}

void Communication::waitForRadioModule(RF24 radio){
	while(!setupComms(radio)) { //Has the radio module initialised?
    Serial.println("RF24 Fail"); //Nope, in this case user needs to check pins
  }
    Serial.println("RF24 Up!"); // Yes it has
}
