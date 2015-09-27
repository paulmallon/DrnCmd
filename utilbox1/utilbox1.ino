//#include <MIDI.h>
//MIDI_CREATE_DEFAULT_INSTANCE();



const int led = 9;      // LED connected to digital pin 9
const int button1 = 2;   // potentiometer connected to analog pin 3
const int teleJack1 = 3;   // potentiometer connected to analog pin 3
const int ledVal = 500; // default led value



 // -----------------------------------
 
 
void setup() {
 // MIDI.begin(8) ;
  Serial.begin(9600);
  
  //set upp midi handles
  //MIDI.setHandleControlChange(OnControlChange);
    
  // set pin modes  
  pinMode(button1, INPUT_PULLUP);
  pinMode(teleJack1, INPUT_PULLUP);     
  pinMode(led, OUTPUT);   
  analogWrite(led, 50);  
}



void loop() {
   
 // MIDI.read(); 
   
  int buttonVal = digitalRead(button1);   
  int teleJackVal = digitalRead(teleJack1);   

  
  
  if(buttonVal == 0 ) 
  {
   Serial.println ("Button pressed");
   analogWrite(led, 0);
   delay(1000);
  }  
  
  if(teleJackVal == 0 ) 
  {
   Serial.println ("Input form tele jack ");
  }  
  
  
      //  MIDI.sendControlChange(0,127,10); //cc1  on chan 10 
     
  
  


}


void OnControlChange(byte channel, byte control, byte value){ 
  
}


  
