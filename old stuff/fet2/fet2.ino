#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=75,158.27273845672607
AudioOutputI2S           i2s1;           //xy=297.09092712402344,215.27273559570312
AudioConnection          patchCord1(waveform1, 0, i2s1, 0);
AudioConnection          patchCord2(waveform1, 0, i2s1, 1);
AudioControlSGTL5000     audioShield;     //xy=152.25,82.25
// GUItool: end automatically generated code

//----------------------------------------------------------------------
// COnstants  

// Analog set up
// 16bit DAC on Teendy 3.1 = 65535values
// resolution default = 4
boolean useAnalogAvarageReading = true;
const int analogResolution = 16; //  8 =  256, 9 = 512, 10 = 1024, 11= 2048, 12 for 4096 values etc  

// Digital pins
const int pin1 = 5;
const int pin2 = 4;
const int pin3 = 3;
const int pin4 = 2;

// Analog pins
const int analogInPin1 = A2;
const int analogInPin2 = A3;
const int analogInPin3 = A6;
const int analogInPin4 = A7;


//----------------------------------------------------------------------
// variables

int sensorValue = 0;         // the sensor value
int sensorMin = 513;        // minimum sensor value
int sensorMax = 0;           // maximum sensor value
int sound = 0;


//----------------------------------------------------------------------
// SETUP 

void setup(void) {
    //output serial stuff ad 96k 
    Serial.begin(9600);  
    
    
    // Init audio  stufff
    AudioNoInterrupts();
    AudioMemory(18);
    audioShield.enable();
    audioShield.volume(0.3);
    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
    AudioInterrupts();
  
   
    // Setup didigtal pins
    pinMode(pin1, INPUT_PULLUP); // Pushbutton
    pinMode(pin2, INPUT_PULLUP); // Pushbutton
    pinMode(pin3, INPUT_PULLUP); // Pushbutton
    pinMode(pin4, OUTPUT); // LED 
   
    // Setup analog pin behaviour
     analogReadResolution(analogResolution); 
     
    // Avaragin may slow down ADC ? 
     if(useAnalogAvarageReading) {
         analogReadAveraging(16); 
     }
 
    // init waveform
    waveform1.pulseWidth(0);
    waveform1.begin(1, 10, WAVEFORM_SQUARE); // Waveform switch and feq selector
    
    // set led to low
    digitalWrite(pin4, LOW);
}


void loop() { 

 //----------------------------------------------------------------------
// Read the analog sensors

  int analogVal =  analogRead(analogInPin1); // Freq
  int analogVa2 =  analogRead(analogInPin2); // Type
  int analogVa3 =  analogRead(analogInPin3); // ??
  int analogVa4 =  analogRead(analogInPin4); // ?? 
    
  //----------------------------------------------------------------------
  // Read the analog sensors
  // Map to freq range
  int freq =  map(analogVal , 0, 65535, 10, 2000 );
  waveform1.frequency(freq);


//int phase = map(analogVa3 , 0, 65535, 0, 360 );
//waveform1.phase(phase);
//Serial.println(phase);

//int time = map(analogVa4 , 0, 65535, 10, 5000 );
//Serial.println(angle);
//waveform1.pulseWidth(angle);

  //float w;
//  for (uint32_t i =1; i<20; i++) {
 //   w = i / 20.0;
 //   waveform1.pulseWidth(w);
//delay(200); 
 // }
  
  

//----------------------------------------------------------------------
//  Switch sound

int newSound = map(analogVa2 , 0, 65535, 1, 4 );

  if(newSound != sound){
    sound = newSound;   
      switch (sound) {
      case 1:
         Serial.println("***** Waveform SINE selected");
         waveform1.begin(1, freq, WAVEFORM_SINE);

        break;
      case 2:
         Serial.println("***** waveform SAWTOOTH selected");
         waveform1.begin(1, freq, WAVEFORM_SAWTOOTH);
 
        break;
        
        case 3:
         Serial.println("***** Waveform SQUARE selected");
         waveform1.begin(1, freq, WAVEFORM_SQUARE);

        break;
        
        case 4:
        Serial.println("***** Waveform TRIANGLE selected");
         waveform1.begin(1, freq, WAVEFORM_TRIANGLE);
        break; 
    }
  }

//----------------------------------------------------------------------
//  Switches

if (!digitalRead(pin1)) {
   Serial.println("*****  1 selected");
   
  } 
  
  if (!digitalRead(pin2)) {
    Serial.println("***** 2 selected");
    
  }
  
  if (!digitalRead(pin3)) {
    Serial.println("*****  3 selected");
    
  } 
  
//  if (!digitalRead(pin4)) {
//    Serial.println("***** 44 TRIANGLE selected");
//  } 
   
  
  

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

}


