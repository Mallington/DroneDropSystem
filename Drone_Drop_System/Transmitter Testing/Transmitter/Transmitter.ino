 #include <SPI.h>
    #include "nRF24L01.h"
    #include "RF24.h"
 
    RF24 radio(9,10);
 
    const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
    unsigned long Command ;
    void setup()
    {
    Serial.begin(57600);
    Serial.println("Module is up!");
 pinMode(4,INPUT);
    radio.begin();
    radio.setRetries(15,15);
    radio.openReadingPipe(1,pipes[1]);
    radio.startListening();
    radio.printDetails();
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
    radio.stopListening();
    }
 
    void loop(void)
    {
    radio.stopListening();
    if (digitalRead(4)==1){
    Command=1;
    }
    else{
      Command=2;
     }
      radio.write( &Command, sizeof(unsigned long) );
    radio.startListening();

    delay(1000);
    }
