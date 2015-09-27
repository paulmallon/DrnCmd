
int ledPin = 9;      // LED connected to digital pin 9

int buttonPin = 2;   // potentiometer connected to analog pin 3


int buttonPin2 = 3;   // potentiometer connected to analog pin 3

int val = 0;         // variable to store the read value


int vala = 0;         // variable to store the read value

int vala2 = 0;         // variable to store the read value

void setup()

{
  
  Serial.begin(9600);
  
  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(buttonPin2, INPUT_PULLUP);
     
         

 pinMode(ledPin, OUTPUT);   // sets the pin as output

    analogWrite(ledPin, 40);  
}



void loop()

{

  vala = digitalRead(buttonPin);   


  vala2 = digitalRead(buttonPin2);   


 Serial.println(vala);


  
  if(vala == 0 ||  vala2 == 0 ) {
  val = 500;
  analogWrite(ledPin, val / 4);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  delay(100);
  
    analogWrite(ledPin, 0);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255   
    analogWrite(ledPin, val / 2);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
      delay(100);

} else {
      analogWrite(ledPin, 600);  
  delay(500);
}
}

  
