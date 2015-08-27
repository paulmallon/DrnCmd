#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveformMod;      //xy=221.09091186523438,282.0909118652344
AudioSynthWaveformSine   sine;          //xy=311.0909118652344,512.0909423828125
AudioSynthWaveform       waveform;      //xy=312.0909118652344,434.0909423828125
AudioSynthWaveformSineModulated sine_fm;       //xy=365.0909423828125,366.0909729003906
AudioMixer4              mixer;         //xy=559.0909118652344,433.0909118652344
AudioOutputI2S           i2s;           //xy=765.0909729003906,403.27276611328125
AudioConnection          patchCord1(waveformMod, sine_fm);
AudioConnection          patchCord2(sine, 0, mixer, 2);
AudioConnection          patchCord3(waveform, 0, mixer, 1);
AudioConnection          patchCord4(sine_fm, 0, mixer, 0);
AudioConnection          patchCord5(mixer, 0, i2s, 0);
AudioConnection          patchCord6(mixer, 0, i2s, 1);
AudioControlSGTL5000     audioShield;     //xy=634.2500915527344,274.25
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
    
    sine.amplitude(1);
    sine.frequency(4400);
    
    sine_fm.amplitude(1);
    sine_fm.frequency(4400);
    
    waveformMod.pulseWidth(0.5);
    waveformMod.begin(1, 100, WAVEFORM_SQUARE); // Waveform switch and feq selector
    
    waveform.pulseWidth(0.7);
    waveform.begin(1, 100, WAVEFORM_PULSE); // Waveform switch and feq selector
    
    
    mixer.gain(0,0);
    mixer.gain(1,0);
    mixer.gain(2,0);
    mixer.gain(3,0); // not used
                
    
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
  int freq1 =  map(analogVal , 0, 65535, 10, 2000 );
 int freq2 =  map(analogVa2 , 0, 65535, 10, 2000 );
 int freq3 =  map(analogVa3 , 0, 65535, 10, 2000 );
 int freq4 =  map(analogVa4 , 0, 65535, 10, 2000 );
 
  
    waveformMod.frequency(freq1);      
    sine.frequency(freq2);          
    waveform.frequency(freq3);     
    sine_fm.frequency(freq4); 
    
    
    
    if (!digitalRead(pin1)) {
   Serial.println("*****  1 selected");
    mixer.gain(0,1);
    mixer.gain(1,0);
    mixer.gain(2,0);
   
  } 
  
  if (!digitalRead(pin2)) {
    Serial.println("***** 2 selected");
     mixer.gain(0,0);
    mixer.gain(1,1);
    mixer.gain(2,0);
    
  }
  
  if (!digitalRead(pin3)) {
    Serial.println("*****  3 selected");
     mixer.gain(0,0);
    mixer.gain(1,0);
    mixer.gain(2,1);
    
  } 
  
  
}
