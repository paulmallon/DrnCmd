#include <SPI.h>
#include <SD.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <MIDI.h>

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

// filter settings
const int maxFilterResonance = 5;
const int maxFilterFrequency = oscMaxFrequency + 500;
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
boolean isOsc2WaveFormTriangle = true;
float outPutVolume = 0.5;

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
  int newIntVal, tempVal = 0;
  float newFloatVal = 0;
    
  switch (control) { 
    case osc1CC: //osc1 freq
       newIntVal = map(value, 0, 127,0, oscMaxFrequency);
       waveformOsc1.frequency(newIntVal);
    break;
    
    case osc2CC: //osc2 freq
       newIntVal = map(value, 0, 127,0, oscMaxFrequency);
       waveformOsc2.frequency(newIntVal);
    break;
  
    case oscMixCC: //osc mix
      newFloatVal = mapfloat(value, 0, 127,0, 1);
      oscMixer.gain(0, newFloatVal);
      oscMixer.gain(1, 1-newFloatVal);           
    break;
  
    case lfo1CC: // lfo 1 speed
      newIntVal = map(value, 0, 127,0, maxLfo1Speed);
      waveformLfo1.frequency(newIntVal);
    break;

    case lfo2CC: // lfo 2 speed - 2, 4, 8, 16
      tempVal = map(value, 0, 127,0, maxLfo1Speed);
      newIntVal = map(value, 0, 127,1, maxLfo2Speed); 
      waveformLfo2.frequency(pow(lfo2Base,newIntVal) * tempVal);
    break;
    
    case shapeCC: //lfo effect on  filter
       newFloatVal = mapfloat(value, 0, 127,0, 1);
       lfoMixer.gain(0, newFloatVal);
    break;
  
    case pulseCC: // lfo depth of modulation of  filter
        newFloatVal = mapfloat(value, 0, 127, 0, 1);      
        waveformLfo2.amplitude(newFloatVal);
    break;
  
    case filterFrequencyCC: // filter frequency
       newIntVal = map(value, 0, 127, 0, maxFilterFrequency );
       filter.frequency(newIntVal);
    break;
  
    case filterResonanceCC: // filter resonance
      newFloatVal = mapfloat(value, 0, 127, minFilterResonance, maxFilterResonance );
      filter.resonance(newFloatVal);
    break;

    case volumeCC: // set audioSheield volume
      newFloatVal = mapfloat(value, 0, 127, 0, volumeCC );
      audioShield.volume(newFloatVal);
    break;

    case osc1WaveformCC: // change osc 1 waveform
     newIntVal = map(value, 0, 127, 0, 1 );

    if(newIntVal == 0) {     
      if(isOsc1WaveformSquare) {    
          //change to trigangle
          waveformOsc1.begin(WAVEFORM_TRIANGLE);
          isOsc1WaveformSquare = false;
      } else {       
          // do nothing   - already set to square     
      }
    }
    break;

    case osc2WaveformCC: // change osc2 waveform
      newIntVal = map(value, 0, 127, 0, 1 );

      if(newIntVal == 0) {     
        if(isOsc2WaveFormTriangle) {    
            //change to trigangle
            waveformOsc2.begin(WAVEFORM_SQUARE);
            isOsc2WaveFormTriangle = false;
       } else {       
            // do nothing - already set to triangle       
      }
    } 
    break;
  }
}

// ------------------------------------------------------
// Main 
void loop() {
  usbMIDI.read(); 
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

   // init waveform for osc 2
    waveformOsc2.pulseWidth(0);
    waveformOsc2.begin(1, osc2Frequency, osc2Waveform); 

    // init lfo waveforms
    waveformLfo1.pulseWidth(0);
    waveformLfo1.begin(1, lfo1Speed , waveformLfoSawtooth);  //saw lfo
    
    // init sub lfo 
    waveformLfo2.pulseWidth(0);
    waveformLfo2.begin(1, lfo2Speed , waveformLfoSquare); // square lfo

    // set mixer  
    oscMixer.gain(0, 0.5);
    oscMixer.gain(1, 0.5);

    // set mix for lfo
    lfoMixer.gain(0, 0);
    
    //filter things
    filter.frequency(filterFrequency);
    filter.resonance(filterResonance);
  }



  


  //----------------------------------------------------------------------
  // Read the analog sensors
//
//  int analogVal1 =  analogRead(analogInPin1); // osc freq
//  int analogVal2 =  analogRead(analogInPin2); // osc freq 2
//  int analogVal3 =  analogRead(analogInPin3); // ??
//  int analogVal4 =  analogRead(analogInPin4); // ?? 
//  int analogVal4 =  analogRead(analogInPin4); // ?? 
//  int analogVal4 =  analogRead(analogInPin4); // ?? 


  //----------------------------------------------------------------------
  // Read the analog sensors
  // Map to freq range
 
//    int freq1 =  map(analogVal1 , 0, 4096, 10, 370 );
//    int freq2 =  map(analogVal2 , 0, 4096, 10, 370 );
//   
//    float oscMixer =  mapfloat(analogVal3 , 0, 4096, 0, 10);
//    int lfo2Speed =  map(analogVal3, 0, 4096, 0, 24);
//    int lfo2Speed =  map(analogVal3, 0, 4096, 0, 24);

