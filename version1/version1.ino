#include <SPI.h>
#include <SD.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <MIDI.h>



const int analogInPin1 = A2;
const int analogInPin2 = A3;
const int analogInPin3 = A6;
const int analogInPin4 = A7;

// ------------------------------------------------------
// GUItool: begin automatically generated code
AudioSynthWaveform       waveformLfo1;   
AudioSynthWaveform       waveformLfo2;  
AudioSynthWaveform       waveformOsc1; 
AudioSynthWaveform       waveformOsc2; 
AudioEffectMultiply      multiply1; //xy=397.407413482666,450.7407455444336
AudioMixer4              oscMixer;  //xy=517.2222137451172,151.72222137451172
AudioMixer4              lfoMixer;  //xy=588.8888893127441,416.3888854980469
AudioFilterStateVariable filter;  //filter      //xy=779.9074230194092,264.9073944091797
AudioOutputI2S           i2s1;     //audio out      //xy=1300.888916015625,362.8888854980469
AudioConnection          patchCord1(waveformLfo1, 0, multiply1, 0);
AudioConnection          patchCord2(waveformLfo2, 0, multiply1, 1);
AudioConnection          patchCord3(waveformOsc1, 0, oscMixer, 0);
AudioConnection          patchCord4(waveformOsc2, 0, oscMixer, 1);
AudioConnection          patchCord5(multiply1, 0, lfoMixer, 0);
AudioConnection          patchCord6(oscMixer, 0, filter, 0);
AudioConnection          patchCord7(lfoMixer, 0, filter, 1);
AudioConnection          patchCord8(filter, 1, i2s1, 0);
AudioControlSGTL5000     audioShield;     //xy=152.25,82.25
// GUItool: end automatically generated code




//----------------------------------------------------------------------
// Constants  


// midi contolchange numbers 
const int osc1CC = 22;
const int osc2CC = 23;
const int oscMixCC = 24;
const int lfo1CC = 25;
const int lfo2CC = 26;
const int shapeCC = 27;
const int pulseCC = 28;
const int filterFrequencyCC = 29;
const int filterResonanceCC = 30;
const int volumeCC = 31;

const int osc1WaveformCC = 14;
const int osc2WaveformCC = 15;

// osc & lfo settings
const int oscMaxFrequency  = 375;
const int maxLfo1Speed = 20;
const int maxLfo2Speed = 16;
const int lfo2Base = 2;
const int maxLfo2SpeedExponent = 4;

// filter settings
const int maxFilterResonance = 5;
const float minFilterResonance = 0.7;

// lfo waveforms
const int waveformLfoSawtooth = WAVEFORM_SAWTOOTH;
const int waveformLfoSquare = WAVEFORM_SQUARE;

//osc waveforms
int oscWaveformSquare = WAVEFORM_SQUARE; 
int oscWaveformTriangle = WAVEFORM_TRIANGLE;


//----------------------------------------------------------------------
// variables
int oscMix = 0;
int lfo1Speed = 0;
int lfo2Speed = 2; 
int shape = 0;
int pulse = 0;
int filterFrequency =0;
int filterResonance = 0;
int osc1Frequency = 0;
int osc2Frequency = 0;
int osc1Waveform = WAVEFORM_SQUARE; 
int osc2Waveform = WAVEFORM_TRIANGLE;
boolean isOsc1WaveformSquare = true;
boolean isOsc1WaveformTriangle = true;

boolean isOsc2WaveformSquare = true;
boolean isOsc2WaveformTriangle = true;

float outPutVolume = .7;

// use analog sensors 

boolean useAnalog = false;
boolean useWideFilterRange = false;
int maxFilterFrequency = 5000;

// ------------------------------------------------------
// init audio, midi and other stuff
void setup() {
    //set upp midi handles
    usbMIDI.setHandleControlChange(OnControlChange);
    
    //output serial stuff ad 96k 
    Serial.begin(9600);  
    
    
    // Init audio  stufff
    initAudioStuff();
    
    // init everything else
    initWaveformsAndOtherThings();  
}


// ------------------------------------------------------
// Read midi cc data

void OnControlChange(byte channel, byte control, byte value){ 
  Serial.println("Midi cc input");
  int newIntVal = 0;
  float newFloatVal = 0;

  int controler = control + 1;

  switch (controler) { 
    case osc1CC: //osc1 freq
    Serial.println("osc1CC");
       newIntVal = map(value, 0, 127,0, oscMaxFrequency);
       waveformOsc1.frequency(newIntVal);
       Serial.println(newIntVal);
    break;
    
    case osc2CC: //osc2 freq
      Serial.println("osc2CC");
       newIntVal = map(value, 0, 127,0, oscMaxFrequency);
       waveformOsc2.frequency(newIntVal);
       Serial.println(newIntVal);
    break;
  
    case oscMixCC: //osc mix
      Serial.println("oscMixCC");
      newFloatVal = mapfloat(value, 0, 127,0, 1);
      oscMixer.gain(0, newFloatVal);
      oscMixer.gain(1, 1-newFloatVal);
      Serial.println("Osc 1 gain");
      Serial.println(newFloatVal); 
      Serial.println("Osc 2 gain");
      Serial.println(1-newFloatVal);           
    break;
  
    case lfo1CC: // lfo 1 speed
      Serial.println("lfo1CC");
      newIntVal = map(value, 0, 127,0, maxLfo1Speed);
      waveformLfo1.frequency(newIntVal);
      Serial.println(newIntVal);
    break;

    case lfo2CC: // lfo 2 speed - 2, 4, 8, 16
      Serial.println("lfo2CC");
      Serial.println("Mapped value");
      newIntVal = map(value, 0, 127,0, maxLfo2SpeedExponent);
      Serial.println(newIntVal);
      Serial.println("Converted value");
      newIntVal = pow(lfo2Base,newIntVal);
      Serial.println(newIntVal);
      waveformLfo2.frequency(newIntVal);
      
    break;
    
    case shapeCC: //lfo effect on  filter
    Serial.println("shapeCC");
       newFloatVal = mapfloat(value, 0, 127,0, 1);
       lfoMixer.gain(0, newFloatVal);
       Serial.println(newFloatVal);
    break;
  
    case pulseCC: // lfo depth of modulation of  filter
        Serial.println("pulseCC");
        newFloatVal = mapfloat(value, 0, 127, 0, 1);      
        waveformLfo2.amplitude(newFloatVal);
        Serial.println(newFloatVal);
    break;
  
    case filterFrequencyCC: // filter frequency
    Serial.println("filterFrequencyCC");
       newIntVal = map(value, 0, 127, 0, maxFilterFrequency );
       filter.frequency(newIntVal);
       Serial.println(newIntVal);
    break;
  
    case filterResonanceCC: // filter resonance
    Serial.println("filterResonanceCC");
      newFloatVal = mapfloat(value, 0, 127, minFilterResonance, maxFilterResonance );
      filter.resonance(newFloatVal);
      Serial.println(newFloatVal);
    break;

    case volumeCC: // set audioSheield volume
      Serial.println("volumeCC");
      newFloatVal = mapfloat(value, 0, 127, 0, 1 );
      audioShield.volume(newFloatVal);
      Serial.println(newFloatVal);
    break;

    case osc1WaveformCC: // change osc 1 waveform
      Serial.println("osc1WaveformCC");
      newIntVal = map(value, 0, 127, 0, 10 );
      Serial.println(newIntVal);  
      if(newIntVal >= 5) {
        if(isOsc1WaveformSquare) {
          Serial.println("change osc 1 to triangle");    
          waveformOsc1.begin(WAVEFORM_TRIANGLE);
          isOsc1WaveformSquare = false;
          isOsc1WaveformTriangle = true;
        }
      } else {
        if(isOsc1WaveformTriangle) {
            Serial.println("change osc 1 to square");
            waveformOsc1.begin(WAVEFORM_SQUARE);

            isOsc1WaveformSquare = true;
            isOsc1WaveformTriangle = false;
        }
      }

    
    break;

    case osc2WaveformCC: // change osc2 waveform
    Serial.println("osc2WaveformCC");
      newIntVal = map(value, 0, 127, 0, 10 );
      Serial.println(newIntVal);  
      if(newIntVal >= 5) {
        if(isOsc2WaveformSquare) {
          Serial.println("change osc 2 to triangle");    
          waveformOsc2.begin(WAVEFORM_TRIANGLE);
          isOsc2WaveformSquare = false;
          isOsc2WaveformTriangle = true;
        }
      } else {
        if(isOsc2WaveformTriangle) {
            Serial.println("change osc 2 to square");
            waveformOsc2.begin(WAVEFORM_SQUARE);

            isOsc2WaveformSquare = true;
            isOsc2WaveformTriangle = false;
        }
      }

    break;
  }


  Serial.println(controler);
  Serial.println(value);

}

// ------------------------------------------------------
// Main 
void loop() {
  usbMIDI.read(); 
   
  if(useAnalog) {
    readAnalog();
  }
}

// ------------------------------------------------------
// Helpers

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// ------------------------------------------------------
// Inits

void initAudioStuff() {
  AudioNoInterrupts();
    AudioMemory(18); // audio needs memory 
    audioShield.enable();
    audioShield.volume(outPutVolume);
    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
    AudioInterrupts();
}
void initWaveformsAndOtherThings() {
  
    // init waveform for osc 1
    waveformOsc1.pulseWidth(0);
    waveformOsc1.begin(1, osc1Frequency, osc1Waveform); 

    isOsc1WaveformSquare = true;
    isOsc1WaveformTriangle = false;

   // init waveform for osc 2
    waveformOsc2.pulseWidth(0);
    waveformOsc2.begin(1, osc2Frequency, osc2Waveform); 
    
    isOsc2WaveformSquare = false;
    isOsc2WaveformTriangle = true;
    // init lfo waveforms
    waveformLfo1.pulseWidth(0);
    waveformLfo1.begin(1, lfo1Speed , waveformLfoSawtooth);  //saw lfo
    
    // init sub lfo 
    waveformLfo2.pulseWidth(0);
    waveformLfo2.begin(1, lfo2Speed , waveformLfoSquare); // square lfo

    // set mixer  
    oscMixer.gain(0, 1);
    oscMixer.gain(1, 1);

    // set mix for lfo
    lfoMixer.gain(0, 1);

    if(useWideFilterRange) {
     maxFilterFrequency = 10000;
    } else {
       maxFilterFrequency = 5000;
    }
    
    //filter things
    filter.frequency(filterFrequency);
    filter.resonance(filterResonance);
  }



  

void readAnalog() {
  // Read the analog sensors
  int analogVal1 =  analogRead(analogInPin1); // osc freq
  int analogVal2 =  analogRead(analogInPin2); // osc freq 2
  int analogVal3 =  analogRead(analogInPin3); // lfo speeds
  int analogVal4 =  analogRead(analogInPin4); // filter frequency 

  // osc frequency
  int freq1 =  map(analogVal1 , 0, 4096, 0, oscMaxFrequency );
  waveformOsc1.frequency(freq1);
  
  int freq2 =  map(analogVal2 , 0, 4096, 0, oscMaxFrequency );
  waveformOsc2.frequency(freq2);

//  osc mixer
//  float oscMixer =  mapfloat(analogVal3 , 0, 4096, 0, 1);
//  oscMixer.gain(0, oscMixer);
//  oscMixer.gain(1, 1-oscMixer);           
  
  // lfo 1
   int lfo1Speed = map(analogVal3 , 0, 4096,0, maxLfo1Speed);
   waveformLfo1.frequency(lfo1Speed);
     
  // lfo 2
  int lfo2Speed = map(analogVal3, 0, 4096,1, maxLfo2Speed); 
  lfo2Speed = pow(lfo2Base,lfo2Speed) * lfo1Speed;
  waveformLfo2.frequency(lfo2Speed);

  // filter frequency
  int filterFrequency = map(analogVal4, 0, 4096,0, maxFilterFrequency); 
  filter.frequency(filterFrequency);

if (Serial) {
  // print stuff to serial bus
  //delay(500);
//Serial.println( ("osc 1 frequency: " +freq1));
//Serial.println(("osc 2 frequency: " + freq2));
//Serial.println(("lfo 1 Speed: " + lfo1Speed));
//Serial.println(("lfo 2 Speed: " + lfo2Speed));
//Serial.println(("filter Frequency: " + filterFrequency));

Serial.println(freq1);
Serial.println(freq2);
Serial.println(lfo1Speed);
Serial.println(lfo2Speed);
Serial.println(filterFrequency);

Serial.println(("---------------------------------------------"));
 delay(200);  // do not print too fast!
 
}

  
}
 

