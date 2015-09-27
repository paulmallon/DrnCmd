
#include <MIDI.h>

#include <SPI.h>
#include <SD.h>




const int led = 9;      // LED connected to digital pin 9
const int button1 = 2;   // potentiometer connected to analog pin 3
const int teleJack1 = 3;   // potentiometer connected to analog pin 3
const int ledVal = 500; // default led value



 // -----------------------------------
 
 
void setup() {
  
  Serial.begin(9600);
  
  //set upp midi handles
  usbMIDI.setHandleControlChange(OnControlChange);
    
  // set pin modes  
  pinMode(button1, INPUT_PULLUP);
  pinMode(teleJack1, INPUT_PULLUP);     
  pinMode(led, OUTPUT);   
  analogWrite(led, 40);  
}



void loop() {
   
  usbMIDI.read(); 
   
  int buttonVal = digitalRead(button1);   
  int teleJackVal = digitalRead(teleJack1);   

  if(buttonVal == 0 ||  teleJackVal == 0 ) {
    analogWrite(ledPin, ledVal / 4);  
    delay(100);
    analogWrite(ledPin, 0);  
    delay(50);
    analogWrite(ledPin, ledVal / 2);  
    delay(100);  
  } else {
        analogWrite(ledPin, ledVal);  
        delay(500);
  }
}


void OnControlChange(byte channel, byte control, byte value){ 
  
  }


  
