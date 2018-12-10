byte POD[6] = "D_POD"; //This is the physical address for the pod
byte TRANSMITTER[6] = "TRANS"; // Physical address for the transmitter/ base station

/*This class was created to be implemented in the firmware of both the base station (transmitter) and drop pod
*Because of this, you will see reference to isBaseSation = true or false. True means that it will act as the transmitter and false means it will act as the drop pod (reciever)
*This class directly interfaces with RF24 library to allow the RF24 to be read and written from, aswell as providing additional functionality
*/
class Communication {
//Its a weird language quirk, it wants all of the methods and data fields told to it before it acutally recieves them
private:
  boolean radioModuleUp = false; // States whether the RF24 module has been initialised yet
  boolean isBaseStation = false; // States whether the current device that this class is being called from is the base station (transmitter) or drop pod
  boolean listening = false; // States whether the RF24 module has been put into listening mode, a mode which allows incoming packets to be listened to
public:
  void writeFloat(float f, RF24 radio);
  void pong(float pongValue, RF24 radio);
  boolean ping(int timeoutMillis, RF24 radio);
  boolean incoming(RF24 radio);
  boolean setupComms(RF24 radio);
  boolean isModuleUp();
  Communication(boolean thisIsBaseStation);
  void waitForRadioModule(RF24 Module);
  float* read(int bufferSize, RF24 radio);
  void write(float buffer[], int bufferSize, RF24 radio);
  void setListening(boolean listen, RF24 radio);
};

//Changes the mode of the RF24 module, (listen = True) - actively listens for incoming packets, (listen = False) - stops listening
void Communication::setListening(boolean listen, RF24 radio){
	listening = listen;
	if(listening) radio.startListening(); //Lets keep our ears peeled for packets
	else radio.stopListening(); //Lets put our fingers in our ears
}

//This method initialises the RF24 module and assigns addresses that act like IP addresses to read and write packets to, if this operation is sucessful, boolean true will be returned
boolean Communication::setupComms(RF24 radio){
  radioModuleUp = radio.begin(); //Initialsies the module

if(radioModuleUp){ // Did the module initialise ok?
	//Yes it did. If not please check wiring
  if(isBaseStation){ //Simply swaps the read and write end addresses so that they match up with each other (eg. 1->2 for write, 2-> read and the other way round for the drop pod)
  	//We are currently acting as the transmitter/ basesation
    radio.openWritingPipe(POD); // Lets write packets to the pod
    radio.openReadingPipe(1,TRANSMITTER); //Lets recieve packets as the transmitter
  }
  else{
  	//We are currently acting as the drop pod
    radio.openWritingPipe(TRANSMITTER); // Lets write packets to the transmitter
    radio.openReadingPipe(1,POD); // Lets reciever packets as the pod
  }
  radio.startListening(); //Lets keep our ears peeled for incoming packets
  listening = true; // Lets let everyone know we are in listening mode
  radio.setPALevel(RF24_PA_LOW); // Sets the power level of the transmitter (Distance^2 ∝ power) : Need to experiment
}
  return radioModuleUp;
}

//Returns a boolean stating whether the RF24 was succesfully booted up
boolean Communication :: isModuleUp(){ // Did the module start up okay?
  return radioModuleUp;
}

Communication::Communication(boolean thisIsBaseStation) // Main initialiser for object
{
	
	isBaseStation = thisIsBaseStation; // States whether it is the the central transmitter or not
}

//This method simply stalls arduino until the RF24 module has started up
void Communication::waitForRadioModule(RF24 radio){
	while(!setupComms(radio)) { //Has the radio module initialised?
    //Serial.println("RF24 Fail"); //Nope, in this case user needs to check pins
  }
   // Serial.println("RF24 Up!"); // Yes it has
}

//This method reads incoming packets (consisting of a float array of fixed length) and returns it
float* Communication::read(int bufferSize, RF24 radio){
	boolean wasListening = listening;

	if(!listening) { // are we already listening for packets?
	setListening(true,radio); // Nope, lets start listening then or we might miss it.
	} 
	float array[bufferSize];
	radio.read( array, sizeof(float)*bufferSize); //reads data being transmitted from remote transmitter into a float array of fixed length
	return array;

	if(listening != wasListening) setListening(wasListening, radio); //Sets transmitter back to previous state
}

boolean Communication::incoming(RF24 radio){
	return (radio.available());
}

boolean Communication::ping(int timeoutMillis, RF24 radio){
	float pingValue = 0.0f;
	float startTime = millis();
	boolean timeout= false;

	float packet[1] = {pingValue};
	write(packet, 1, radio);

	setListening(true, radio);

	while(!incoming(radio) && !(timeout = ((millis()-startTime)>timeoutMillis)));

	if(!timeout){
		if(read(1, radio)[0]==pingValue){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}

}
void Communication::writeFloat(float f, RF24 radio){
	float packet[1] = {f};
	write(packet, 1, radio);
}
void Communication::pong(float pongValue, RF24 radio){
	writeFloat(pongValue, radio);
}

//This method transmits an outgoing packet to a remote radio device
 void Communication::write(float buffer[], int bufferSize, RF24 radio){
 	boolean wasListening = listening;

 	if(listening){ // Are we still listening for packets?
 		setListening(false, radio); //Lets concentrate on sending our own packets and stop listening for other peoples, okay?
 	}
 	radio.write(buffer, sizeof(float)*bufferSize); // now we are not listening, let's transmit our own packet

 	if(listening != wasListening) setListening(wasListening, radio); //Sets transmitter back to previous state
 }


