/*
  Analog input, analog output, serial output
 
 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.
 
 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground
 
 created 29 Dec. 2008
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 
 */

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin1 = A2;
const int analogInPin2 = A3;
const int analogInPin3 = A6;
const int analogInPin4 = A7;




void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
}

void loop() {
 

  // print the results to the serial monitor:
  Serial.print("sensor1 = " );                       
  Serial.print(analogRead(analogInPin1));      
   Serial.print("\n" );                       

  Serial.print("sensor2 = " );                       
  Serial.print(analogRead(analogInPin2));      
   Serial.print("\n" );                       

  Serial.print("sensor3 = " );                       
  Serial.print(analogRead(analogInPin3));      
   Serial.print("\n" );                       

   Serial.print("sensor4 = " );                       
  Serial.print(analogRead(analogInPin4));      
   Serial.print("\n" );                       

  delay(1000);                     
}

