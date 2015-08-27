#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "C:\Users\shkval\Desktop\teensy\fet1\floatToString.h"

//WAVEFORM_SINE
//WAVEFORM_SAWTOOTH
//WAVEFORM_SQUARE
//WAVEFORM_TRIANGLE
//WAVEFORM_ARBITRARY
//WAVEFORM_PULSE



 
// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=75,158.27273845672607
AudioOutputI2S           i2s1;           //xy=297.09092712402344,215.27273559570312
AudioConnection          patchCord1(waveform1, 0, i2s1, 0);
AudioConnection          patchCord2(waveform1, 0, i2s1, 1);
AudioControlSGTL5000     audioShield;     //xy=152.25,82.25
// GUItool: end automatically generated code



int sensorValue = 0;         // the sensor value
int sensorMin = 4096;        // minimum sensor value
int sensorMax = 0;           // maximum sensor value


int pin = 0;
int pin2 = 1;
int pin3 = 2;
int pin4 = 3;

int led = 13;


const int analogInPin1 = A2;
const int analogInPin2 = A3;
const int analogInPin3 = A6;
const int analogInPin4 = A7;

char buffer[25];

float superVal = 0;


const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int indez = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int freq = 150;
int sound =0;

void setup(void) {
  
    pinMode(led, OUTPUT);     
    pinMode(pin, INPUT_PULLUP); // Pushbutton
    pinMode(pin2, INPUT_PULLUP); // Pushbutton
    
    pinMode(pin3, INPUT_PULLUP); // Pushbutton
    pinMode(pin4, INPUT_PULLUP); // Pushbutton
   
    AudioMemory(18);
    audioShield.enable();
    audioShield.volume(0.5);
     
    waveform1.pulseWidth(0);
    waveform1.begin(1, freq, WAVEFORM_SQUARE); // Waveform switch and feq selector
    
    Serial.begin(9600); 
    
    
     // initialize all the readings to 0: 
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;   
}


void loop() { 
analogReadResolution(9); // 12 for 4096 values // 8 =  256 // 9 = 512
analogReadAveraging(16); // to do read
 
  
  // subtract the last reading:
  total= total - readings[indez];         
  // read from the sensor:  
  readings[indez] = analogRead(analogInPin1); 
  // add the reading to the total:
  total= total + readings[indez];       
  // advance to the next position in the array:  
  indez = indez + 1;                    

  // if we're at the end of the array...
  if (indez >= numReadings)              
    // ...wrap around to the beginning: 
    indez = 0;                           

  // calculate the average:
  average = total / numReadings;         
  // send it to the computer as ASCII digits
 
 freq =  map(average , 0, 512, 10, 2500 );
  //Serial.println(freq);   
  waveform1.frequency(freq);


//----------------------------------------------------------------------


int newSound  = analogRead(analogInPin2);
  newSound = map(newSound , 0, 511, 1, 4 );
  
  if(newSound != sound){
    sound = newSound;   
    Serial.println(sound);   
      switch (sound) {
      case 1:
         Serial.println("WAVEFORM_SINE");
         waveform1.begin(1, freq, WAVEFORM_SINE);
        break;
      case 2:
         Serial.println("WAVEFORM_SAWTOOTH");
        waveform1.begin(1, freq, WAVEFORM_SAWTOOTH);
        break;
        
        case 3:
         Serial.println("WAVEFORM_SQUARE");
        waveform1.begin(1, freq, WAVEFORM_SQUARE);
        break;
        
        case 4:
        Serial.println("WAVEFORM_TRIANGLE");
         waveform1.begin(1, freq, WAVEFORM_TRIANGLE);
        break;
      default: 
      Serial.println("DEFAULT  - -- -- WAVEFORM_TRIANGLE");
         waveform1.begin(1, freq, WAVEFORM_TRIANGLE);
    }
  }
   
  
//  float w;
//  for (uint32_t i =1; i<20; i++) {
//    w = i / 20.0;
//    waveform1.pulseWidth(w);
//  }


//int freq = analogRead(analogInPin1);
//sensorValue = map(freq , 0, 4096, 10, 2500 );

//if(sensorValue-2 > superVal ||sensorValue+2 < superVal ) {
//  waveform1.frequency(sensorValue);
//  superVal = sensorValue;
 // Serial.println(floatToString(buffer, superVal, 1)); 
//}




//int angle = analogRead(analogInPin2);
//angle = map(angle , 0, 4096, 0, 360 );
//waveform1.phase(angle);
//Serial.println(floatToString(buffer, angle, 1)); 
//Cause the generated waveform to jump to a specific point within its cycle. Angle is from 0 to 360 degrees. When multiple objects are configured, AudioNoInterrupts() should be used to guarantee all new settings take effect together.

//int pulse = analogRead(analogInPin3);
//pulse = map(pulse , 0, 4096, 0, 10000);
//waveform1.pulseWidth(pulse);
//Serial.println(floatToString(buffer, pulse, 1)); 
// Change the width (duty cycle) of the pulse.


  if (digitalRead(pin) == LOW) {
      Serial.println("WAVEFORM_SINE");
     waveform1.begin(1, freq, WAVEFORM_SINE);
 
  }
  
  if (digitalRead(pin2) == LOW) {
          Serial.println("WAVEFORM_SAWTOOTH");
    waveform1.begin(1, freq, WAVEFORM_SAWTOOTH);
 
 
  }
  
  
  if (digitalRead(pin3) == LOW) {
      Serial.println("WAVEFORM_SQUARE");
    waveform1.begin(1, freq, WAVEFORM_SQUARE);
  
  }
  
  if (digitalRead(pin4) == LOW) {
    Serial.println("WAVEFORM_TRIANGLE");
    waveform1.begin(1, freq, WAVEFORM_TRIANGLE);
  }


delay(10);


}


