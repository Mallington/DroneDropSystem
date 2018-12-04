class Led{
private:
  int LED;
  boolean flashing = false;
  boolean lastHigh = false;
  int startTime =0;
  int period =0;
  int lastEpochProg =-1;
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

void Led::on(){
  flashing = false;
  digitalWrite(LED, HIGH);
}
void Led::off(){
  flashing = false;
  digitalWrite(LED, LOW);
}

void Led::progressEpoch(){
  if(flashing){

    int epochProg = (millis()-startTime)/(period/2);
    if(lastEpochProg != epochProg) {
      lastEpochProg = epochProg;
      if(epochProg %2 ==0){
        digitalWrite(LED, HIGH);
        Serial.println("High:"+String(LED));
      }
      else{
        Serial.println("Low: "+String(LED));
        digitalWrite(LED, LOW);
      }
  }
  }
}

void Led::flash(int timePeriod){
  period = timePeriod;
  startTime = millis();
  flashing = true;
}



