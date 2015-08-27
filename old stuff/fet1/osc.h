//////////////////////////////////////////
//  OSCILLATOR FUNCTIONS
//////////////////////////////////////////
  
  
// CHANNEL 1: PWM OSCILLATOR 1  
int ch1_Oscillator(byte dataType, byte outsideMidiNoteNumber, 
   float outsidePitchInMicros, float outsideBendFactor, byte outsideCC) {

   static boolean insideRun = LOW;
   static byte insideMidiNoteNumber = 0;
   static unsigned long insidePitchInMicros = 0;
   static unsigned long insidePitchInMicrosAfterBendFactor = 0;
   static float insideBendFactor = 1.0;
   static byte insideCC1 = 64;
   static byte insideCC3 = 0;
   static byte insideCC5 = 0;
   static boolean insideHigh = 0;
   static unsigned long insidePreviousMicros = 0;
   unsigned long currentMicros;
   static byte insidePreviousMidiNoteNumber;
   static boolean portamentoRunning;
   static boolean portamentoDirection;
   static float portamentoSpeed = 1.0;
   static float portamentoSpeedInMicros = 100.0 / (float)portamentoSpeed;
   static unsigned long previousPortamentoChange;
   static unsigned long insidePreviousPitchInMicros = 0;
   static unsigned long insideTargetPitchInMicros = 0;
   static unsigned long insidePreviousDecayMicros = 0;
   static boolean insideDecay = LOW;
   static unsigned int insideDecayInterval;
   static byte heldNoteNumberArray[16];
   static unsigned long heldNoteNumberDurationMillis[16];
   static byte j = 0;
   static boolean blankSlotFound = LOW;
   static unsigned long newestNoteDuration = 0;
   static byte highestNoteNumber = 0;
   static byte newestNote = 0;
   static float portamentoSpeedConstant = 0.0018;
   static float insidePitchInFrequencyAfterBendFactor;
   static boolean pwmFollowerOn = LOW;
   static unsigned long insidePreviousPwmFollowerMicros;
   static unsigned long pwmFollowerInterval = 50000;
   unsigned long elapsedTimeInMicros;
   float portamentoStepInMicros;
  
   switch(dataType){
      // Run Current Oscillator Settings
      case 1:  
         if (insideRun == HIGH){
                  
            currentMicros = micros();
            // If it's time to decrease the pitch, do so, but scale it 
            // non-linearly so that the speed is always the same as if 
            // the shift is passing through 500 Hz.  If the pitch is 
            // higher than 500 Hz, slow the portamento down.  If the 
            // pitch is lower than 500 Hz, speed the portamento up.

            if ((portamentoRunning == HIGH) && (insideDecay == LOW) && 
               (portamentoDirection == LOW) && 
               ((currentMicros - previousPortamentoChange) >= 
               (float)portamentoSpeedInMicros)) {

               portamentoStepInMicros = (portamentoSpeedConstant / 
                  (1.0 / (float)insidePitchInMicros));
               if (portamentoStepInMicros < 1.0){
                  portamentoStepInMicros = 1;
               }
               insidePitchInMicros = insidePitchInMicros + 
                  portamentoStepInMicros;
               previousPortamentoChange = micros();
              
               insidePitchInMicrosAfterBendFactor = 
                  ((float)insidePitchInMicros * (float)insideBendFactor);
              
               if (insidePitchInMicros >= insideTargetPitchInMicros){
                  portamentoRunning = LOW;
                  insidePitchInMicros = insideTargetPitchInMicros;
               }
            }
            // If it's time to increase the pitch, do so, but scale it 
            // non-linearly so that the speed is always the same as if 
            // the shift is passing through 500 Hz.  If the pitch is 
            // higher than 500 Hz, slow the portamento down.  If the 
            // pitch is lower than 500 Hz, speed the portamento up.

            if ((portamentoRunning == HIGH) && (insideDecay == LOW) && 
                  (portamentoDirection == HIGH) && 
                  ((currentMicros - previousPortamentoChange) >= 
                  (float)portamentoSpeedInMicros)) {

               portamentoStepInMicros = (portamentoSpeedConstant / 
                  (1.0 / (float)insidePitchInMicros));
               if (portamentoStepInMicros < 1.0){
                  portamentoStepInMicros = 1;
               }
               insidePitchInMicros = insidePitchInMicros - 
                  portamentoStepInMicros;
               previousPortamentoChange = micros();
               
               insidePitchInMicrosAfterBendFactor = 
                  ((float)insidePitchInMicros * (float)insideBendFactor);
               
               if (insidePitchInMicros <= insideTargetPitchInMicros){
                  portamentoRunning = LOW;
                  insidePitchInMicros = insideTargetPitchInMicros;
               }
            }
            
            
            if ((portamentoRunning == HIGH) && (insideDecay == HIGH)){
               insidePitchInMicros = insideTargetPitchInMicros;
               portamentoRunning = LOW;
              
               insidePitchInMicrosAfterBendFactor = 
                  ((float)insidePitchInMicros * (float)insideBendFactor);
            }
            
            currentMicros = micros();
            elapsedTimeInMicros = currentMicros - insidePreviousMicros;
            if (elapsedTimeInMicros >= insidePitchInMicrosAfterBendFactor){
               analogWrite(audioOne, (insideHigh == LOW) ? 
                  (insideCC1 * 2) : LOW);
               insideHigh = ~insideHigh;
               insidePreviousMicros = micros();
            }
            
            currentMicros = micros();
            
            if (((currentMicros - insidePreviousPwmFollowerMicros) >= 
               pwmFollowerInterval) && (insideCC1 > 1)) {
               
               if (pwmFollowerOn == HIGH){
                  insidePitchInFrequencyAfterBendFactor = 
                     (2000000.0/(float)insidePitchInMicrosAfterBendFactor);
                  analogWriteFrequency(audioOne, 
                     insidePitchInFrequencyAfterBendFactor);
                  insidePreviousPwmFollowerMicros = micros();
               }
            }
            
            currentMicros = micros();
            
            if ( ((currentMicros - insidePreviousDecayMicros) >= 
               insideDecayInterval) && (insideDecay == HIGH) && 
               (insideCC1 > 0)) {

               insideCC1--;
               insidePreviousDecayMicros = micros();
              
               insidePitchInMicrosAfterBendFactor = 
                  ((float)insidePitchInMicros * (float)insideBendFactor);
            }
         
         return insideMidiNoteNumber;  
         }
         
         if (insideRun == LOW){
            return 0;
         }
             
      break;

      // Note On
      case 2: 
         if ((insideRun == LOW) && (outsideMidiNoteNumber > 0) && 
            (ch1_Arpeggiator(5,0) == LOW)) {
            
            portamentoRunning = LOW;
            insidePitchInMicros = outsidePitchInMicros;
            insidePreviousMidiNoteNumber = 0;
            insideRun = HIGH;
            insideMidiNoteNumber = outsideMidiNoteNumber;
           
            if (isThisNoteInArray(heldNoteNumberArray,outsideMidiNoteNumber) == 
               LOW){
               
               j = 0;
               blankSlotFound = LOW;
               while (blankSlotFound == LOW){
                  if ((heldNoteNumberArray[j] != 0) && (j < 16)){
                     j++;
                  }
                  else {
                     blankSlotFound = HIGH;
                  }
               }
               heldNoteNumberArray[j] = outsideMidiNoteNumber;
               heldNoteNumberDurationMillis[j] = millis();
            }
         }
         
         if ((ch1_Arpeggiator(5,0) == HIGH) && (outsideMidiNoteNumber > 0)) {
            portamentoRunning = LOW;
            insidePitchInMicros = outsidePitchInMicros;
            insidePreviousMidiNoteNumber = 0;
            insideRun = HIGH;
            insideMidiNoteNumber = outsideMidiNoteNumber;
         }
         
         if ((insideRun == HIGH) && (ch1_Arpeggiator(5,0) == LOW) && 
            (outsideMidiNoteNumber > 0)) {
            if (isThisNoteInArray(heldNoteNumberArray,outsideMidiNoteNumber) == 
               LOW) {

               j = 0;
               blankSlotFound = LOW;
               while (blankSlotFound == LOW){
                  if ((heldNoteNumberArray[j] != 0) && (j < 16)){
                     j++;
                  }
                  else {
                     blankSlotFound = HIGH;
                  }
               }
               heldNoteNumberArray[j] = outsideMidiNoteNumber;
               heldNoteNumberDurationMillis[j] = millis();
            }
            j = 0;
            newestNoteDuration = 0;
            for (j = 0; j < 16; j++) {
               if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
                  newestNote = heldNoteNumberArray[j];
               }
            }
            insidePreviousMidiNoteNumber = insideMidiNoteNumber;
            insideMidiNoteNumber = heldNoteNumberArray[j];
            insidePreviousPitchInMicros = insidePitchInMicros;
            insideTargetPitchInMicros = outsidePitchInMicros;
            portamentoRunning = HIGH;
            insideRun = HIGH;
         }
              
              
         if (insideDecay == HIGH){
            insideCC1 = 64;
         }
              
         if (portamentoRunning == HIGH) {
            if (insidePreviousPitchInMicros < insideTargetPitchInMicros) {
               portamentoDirection = LOW; 
               //Going down in pitch.  insidePitchInMicros will grow.
            }
            else {
               portamentoDirection = HIGH; 
               //Going up in pitch.  insidePitchInMicros will shrink.
            }
         }

         insidePitchInMicrosAfterBendFactor = ((float)insidePitchInMicros * 
            (float)insideBendFactor);
         ch1_Arpeggiator(9,0);
      
      break;

      // Note Off
      case 3: 
         if (outsideMidiNoteNumber > 0) {
            j = 0;
            highestNoteNumber = 0;
            for (j = 0; j < 16; j++) {
               if (heldNoteNumberArray[j] == outsideMidiNoteNumber) {
                  heldNoteNumberDurationMillis[j] = 0;
                  heldNoteNumberArray[j] = 0;
               }
               if (heldNoteNumberArray[j] > 0) {
                  highestNoteNumber = heldNoteNumberArray[j];
               }  
            }
            if (highestNoteNumber == 0) {
               insideRun = LOW;
               analogWrite(audioOne, LOW);
               insidePreviousMidiNoteNumber = 0;
            }
            else if (insideDecay == LOW) {
               j = 0;
               newestNoteDuration = 0;
               for (j = 0; j < 16; j++) {
                  if ((heldNoteNumberDurationMillis[j] > newestNoteDuration) &&
                     (insideDecay == LOW)) {
	        
                     newestNote = heldNoteNumberArray[j];
                  }
               }
            
               if ((ch1_Arpeggiator(5,0) == LOW) || ((ch1_Arpeggiator(5,0) == 
                  HIGH) && (ch1_Arpeggiator(7,0) == HIGH) && 
                  (ch1_Arpeggiator(8,0) == LOW))) {

                  OnNoteOn(1,newestNote,127);
               }
            }
          
            insidePitchInMicrosAfterBendFactor = ((float)insidePitchInMicros * 
               (float)insideBendFactor);
            ch1_Arpeggiator(9,
               testArrayContentsForTwoNotesOrMore(heldNoteNumberArray));
         }
      break;
      
      // Pitch Bend
      case 4: 
         insideBendFactor = outsideBendFactor;
         insidePitchInMicrosAfterBendFactor = ((float)insidePitchInMicros * 
            (float)insideBendFactor);
                
      break;
      // CC 1 Change
      case 5: 
         insideCC1 = outsideCC;
         insidePitchInMicrosAfterBendFactor = ((float)insidePitchInMicros * 
            (float)insideBendFactor);
                
      break;
      
      // CC 2 Change
      case 6:
         insidePitchInMicrosAfterBendFactor = ((float)insidePitchInMicros * 
            (float)insideBendFactor);
      break;
      
      // CC 3 Change
      case 7: 
         insideCC3 = outsideCC;
         if (insideCC3 > 0) {
            insideDecay = HIGH;
            insideDecayInterval = map(insideCC3,0,127,10000,1000);
         }
         else {
            insideDecay = LOW;
            insideCC1 = 64;
         }

         insidePitchInMicrosAfterBendFactor = ((float)insidePitchInMicros * 
            (float)insideBendFactor);
      break;
      
      // CC 4 Change
      case 8:
      break;
      
      // CC 6 Change
      case 9:
         insideCC5 = outsideCC;

         if (insideCC5 > 0) {
            pwmFollowerOn = HIGH;
         }
         else {
            pwmFollowerOn = LOW;
            analogWriteFrequency(audioOne,4000);
         }
        
         insidePitchInMicrosAfterBendFactor = ((float)insidePitchInMicros * 
            (float)insideBendFactor);
      break;
      
      // Returns newest note to Arpeggiator when it turns off.
      case 10:
         j = 0;
         newestNoteDuration = 0;
         for (j = 0; j < 16; j++) {
            if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
               newestNote = heldNoteNumberArray[j];
               newestNoteDuration = heldNoteNumberDurationMillis[j];
            }      
         }
        
         insidePitchInMicrosAfterBendFactor = ((float)insidePitchInMicros * 
            (float)insideBendFactor);
        
         return newestNote;
      
      break;
      
      case 11:
         return insideDecay;
      break;

      case 12:
         portamentoSpeedInMicros = map(outsideCC,0,127,20,2000);
      break;
      
           
   }
}






// CHANNEL 2: SQUARE/POLYPULSE OSCILLATOR WITH VIBRATO
int ch2_Oscillator(byte dataType, byte outsideMidiNoteNumber, 
   float outsidePitchInMicros, float outsideBendFactor, byte outsideCC) {

   static boolean insideRun = LOW;
   static byte insideMidiNoteNumber = 0;
   static unsigned long insidePitchInMicros = 0;
   static unsigned long insidePitchInMicrosAfterBendFactor = 0;
   static float insideBendFactor = 1.0;
   static byte insideCC1 = 0;
   static byte insideCC3 = 0;
   static byte insideCC5 = 0;
   static boolean insideHigh = LOW;
   static unsigned long insidePreviousMicros = 0;
   unsigned long currentMicros;
   static byte insidePreviousMidiNoteNumber;
   static boolean portamentoRunning;
   static boolean portamentoDirection;
   static float portamentoSpeed = 1.0;
   static float portamentoSpeedInMicros = 100.0 / (float)portamentoSpeed;
   static unsigned long previousPortamentoChange;
   static unsigned long insidePreviousPitchInMicros = 0;
   static unsigned long insideTargetPitchInMicros = 0;
   static boolean insidePitfall = LOW;
   static int insidePitfallPosition = 0;
   // static int insidePitfallWaveform = B0011101111100110;
   static int insidePitfallWaveform = 0x3BE6;
   static unsigned long insidePitchInMicros2 = 0;
   static byte heldNoteNumberArray[16];
   static unsigned long heldNoteNumberDurationMillis[16];
   static byte j = 0;
   static boolean blankSlotFound = LOW;
   static unsigned long newestNoteDuration = 0;
   static byte highestNoteNumber = 0;
   static byte newestNote = 0;
   static float portamentoSpeedConstant = 0.0018;
   unsigned long currentMillis;
   unsigned long elapsedTimeInMicros;
   
   static float pitfallTuningOffset = 0.87; 
   //Lower the number means lower the pitch of the pitfall polypulse sound.
   
   float portamentoStepInMicros;
              
              
   // Vibrato variables
   static unsigned long previousVibCountMicros = 0;
   static int vibCount = 0;
   static boolean vibDirection = 0; //0 counts up, 1 counts down
   static int vibAmount = 0;
   static float vibAmount2 = 0.0;
   static float vibFactor = 1.0;
   static int modWheelFactor = 0;
   static float finalFrequency = 0;
   unsigned long currentVibCountMicros;
   
   static unsigned long sineVibSpeedStepsInMicros = 2000; 
   // Resolution of the sine vibrato.  (Calculates every x microseconds)
   
   static byte vibWaveform = 1;   
   // 0 is triangle vibrato.  1 is sine vibrato.
   
   static unsigned long triangleVibSpeedStepsInMicros = 320; 
   // Reciprocal of Vib Speed for Triangle Vibrato 
   // (smaller number means faster)
   
   static float vibRateDivisor = 23.0;

   switch(dataType){
      // Run Current Oscillator Settings
      case 1:  
         if (insideRun == HIGH) {
        
            // TRIANGLE VIBRATO - creating vibAmount
            if (vibWaveform == 0){
               if (insideCC1 > 0){
                  if (vibCount <= 0) { // Set direction of count
                     vibDirection = 0; 
                  }
                  if (vibCount >= 200) {
                     vibDirection = 1; 
                  }
                
                  currentVibCountMicros = micros();
              
                  if ((vibDirection == 0) && ((currentVibCountMicros - 
                     previousVibCountMicros) >= ( 
                     (float)triangleVibSpeedStepsInMicros 
                     / (float)vibRateFactor))) {

                     vibCount ++; 
                     // If the proper amount of microseconds have passed 
                     // and we're counting up, count up one.

                     previousVibCountMicros = micros(); 
                     // Set the current time for reference the next time.
                  }
              
                  if ((vibDirection == 1) && ((currentVibCountMicros - 
                     previousVibCountMicros) >= ( 
                     (float)triangleVibSpeedStepsInMicros / 
                     (float)vibRateFactor))) {
                     
                     vibCount --; 
                     // If the proper amount of microseconds have passed 
                     // and we're counting down, count down one.
                     
                     previousVibCountMicros = micros(); 
                     // Set the current time for reference the next time.
                  }
              
                  vibAmount = map(vibCount, 0, 200, -100, 100);
                  
                  vibAmount2 = (float)vibAmount * vibDepth; 
                  //Scales vibAmount to very small positive or negative amounts
                  
                  modWheelFactor = map(insideCC1, 0, 127, 0, 2000);
                  vibFactor = 1.0 + (vibAmount2 * ((float)modWheelFactor/1000));
                
                  insidePitchInMicrosAfterBendFactor = 
                     calculateInsidePitchInMicrosAfterBendFactorCh2(
                        insidePitchInMicros, insideBendFactor, vibFactor, 
                        insidePitfall, pitfallTuningOffset);
              
               }
                
               else { // Squelch IEEE 754 math errors.
                  vibFactor = 1.0;
               }
               
            }
              
              // SINE VIBRATO - creating vibAmount
            if (vibWaveform == 1) {
               if (insideCC1 > 0) { 
                  currentVibCountMicros = micros();
                  if  ((currentVibCountMicros - previousVibCountMicros) >= 
                     sineVibSpeedStepsInMicros) { 
                     // Limits how often the chip can calculate the sine function.

                     currentMillis = millis();
                     vibAmount = sin(((float)currentMillis * 
                        (float)vibRateFactor) / (float)vibRateDivisor) * 100.0;
                        //range of  1 to -1, sclaed to time, 
                        // factored to 100 to -100
   
                     vibAmount2 = (float)vibAmount * vibDepth; 
                        //Scales vibAmount to very small positive or 
                        // negative amounts
   
                     modWheelFactor = map(insideCC1, 0, 127, 0, 2000);
                     vibFactor = 1.0 + (vibAmount2 * 
                        ((float)modWheelFactor/1000));
                  
                     insidePitchInMicrosAfterBendFactor = 
                        calculateInsidePitchInMicrosAfterBendFactorCh2(
                           insidePitchInMicros, insideBendFactor, 
                           vibFactor, insidePitfall, pitfallTuningOffset);
               
                     previousVibCountMicros = micros();
                  }
               }
                                                        
               else { // Squelch IEEE 754 math errors.
                  vibFactor = 1.0;
               }
            }
              
              
            
            // Oscillator guts
                  
            currentMicros = micros(); 
            
            // If it's time to decrease the pitch, do so, but scale it 
            // non-linearly so that the speed is always the same as if 
            // the shift is passing through 500 Hz.  If the pitch is 
            // higher than 500 Hz, slow the portamento down.  If the 
            // pitch is lower than 500 Hz, speed the portamento up.
            if ((portamentoRunning == HIGH) && (portamentoDirection == LOW) && 
               ((currentMicros - previousPortamentoChange) >= 
               portamentoSpeedInMicros)) {
               
               portamentoStepInMicros = (portamentoSpeedConstant / 
                  (1.0 / (float)insidePitchInMicros));
               if (portamentoStepInMicros < 1.0) {
                  portamentoStepInMicros = 1;
               }
               insidePitchInMicros = insidePitchInMicros + 
               portamentoStepInMicros;
               previousPortamentoChange = micros();
                      
               insidePitchInMicrosAfterBendFactor = 
                  calculateInsidePitchInMicrosAfterBendFactorCh2( 
                  insidePitchInMicros, insideBendFactor, vibFactor, 
                  insidePitfall, pitfallTuningOffset);
                
               if (insidePitchInMicros >= insideTargetPitchInMicros){
                  portamentoRunning = LOW;
                  insidePitchInMicros = insideTargetPitchInMicros;
                        
                  insidePitchInMicrosAfterBendFactor = 
                     calculateInsidePitchInMicrosAfterBendFactorCh2( 
                     insidePitchInMicros, insideBendFactor, vibFactor, 
                     insidePitfall, pitfallTuningOffset);
                        
               }
            }
                  
            // If it's time to increase the pitch, do so, but scale it 
            // non-linearly so that the speed is always the same as if 
            // the shift is passing through 500 Hz.  If the pitch is higher 
            // than 500 Hz, slow the portamento down.  If the pitch is lower 
            // than 500 Hz, speed the portamento up.

            if ((portamentoRunning == HIGH) && (portamentoDirection == HIGH) && 
               ((currentMicros - previousPortamentoChange) >= 
               portamentoSpeedInMicros)) {

	            portamentoStepInMicros = (portamentoSpeedConstant / (1.0 / 
                  (float)insidePitchInMicros));
                    
                  if (portamentoStepInMicros < 1.0) {
                     portamentoStepInMicros = 1;
                  }
                  
                  insidePitchInMicros = insidePitchInMicros - 
                     portamentoStepInMicros;
                  previousPortamentoChange = micros();

                  insidePitchInMicrosAfterBendFactor = 
                     calculateInsidePitchInMicrosAfterBendFactorCh2( 
                     insidePitchInMicros, insideBendFactor, vibFactor, 
                     insidePitfall, pitfallTuningOffset);
                  
                  if (insidePitchInMicros <= insideTargetPitchInMicros) {
                     portamentoRunning = LOW;
                     insidePitchInMicros = insideTargetPitchInMicros;
                       
                       
                     insidePitchInMicrosAfterBendFactor = 
                        calculateInsidePitchInMicrosAfterBendFactorCh2( 
                        insidePitchInMicros, insideBendFactor, vibFactor, 
                        insidePitfall, pitfallTuningOffset);                         
              
                  }
            }
                  
                  
            currentMicros = micros();
            elapsedTimeInMicros = currentMicros - insidePreviousMicros;
            if (elapsedTimeInMicros >= insidePitchInMicrosAfterBendFactor) {
               // digitalWrite(audioTwo, ~insideHigh);
               if (insideHigh == LOW) {
                  //PORTC |= (1<<3);
                  digitalWriteFast(audioTwo,HIGH);
               }
               else {
                  //PORTC &= ~(1<<3);
                  digitalWriteFast(audioTwo,LOW);
               }
                      
               if (insidePitfall == HIGH){
                  if ((bool(insidePitfallWaveform & (0x0001 << 
                     insidePitfallPosition))) == HIGH) {
                  
                     insideHigh = ~insideHigh;
                  }
                  insidePitfallPosition ++;
                  if (insidePitfallPosition >= 14){
                     insidePitfallPosition = 0;
                  }
               }
                    
               else {  
                  insideHigh = ~insideHigh;
               }
               insidePreviousMicros = micros(); 
            }
                  
                                   
            return insideMidiNoteNumber;  
         }
         
         if (insideRun == LOW){
            return 0;
         }

      break;
      

      // Note On
      case 2: 
      
         if ((insideRun == LOW) && (outsideMidiNoteNumber > 0) && 
            (ch2_Arpeggiator(5,0) == LOW)) {
            
            portamentoRunning = LOW;
            insidePitchInMicros = outsidePitchInMicros;
            insidePreviousMidiNoteNumber = 0;
            insideRun = HIGH;
            insideMidiNoteNumber = outsideMidiNoteNumber;
                
            if (isThisNoteInArray(heldNoteNumberArray,outsideMidiNoteNumber) ==
               LOW) {

               j = 0;
               blankSlotFound = LOW;
               while (blankSlotFound == LOW) {
                  if ((heldNoteNumberArray[j] != 0) && (j < 16)) {
                     j++;
                  }
                  else {
                     blankSlotFound = HIGH;
                  }
               }
               heldNoteNumberArray[j] = outsideMidiNoteNumber;
               heldNoteNumberDurationMillis[j] = millis();
            }
         }
              
         if ((ch2_Arpeggiator(5,0) == HIGH) && (outsideMidiNoteNumber > 0)) {
            portamentoRunning = LOW;
            insidePitchInMicros = outsidePitchInMicros;
            insidePreviousMidiNoteNumber = 0;
            insideRun = HIGH;
            insideMidiNoteNumber = outsideMidiNoteNumber;
                
         }
              
         if ((insideRun == HIGH) && (ch2_Arpeggiator(5,0) == LOW) && 
            (outsideMidiNoteNumber > 0)) {
                
                
            if (isThisNoteInArray(heldNoteNumberArray,outsideMidiNoteNumber) ==
               LOW) {
               
               j = 0;
               blankSlotFound = LOW;
               while (blankSlotFound == LOW) {
                  if ((heldNoteNumberArray[j] != 0) && (j < 16)) {
                     j++;
                  }
                  else {
                     blankSlotFound = HIGH;
                  }
               }
               heldNoteNumberArray[j] = outsideMidiNoteNumber;
               heldNoteNumberDurationMillis[j] = millis();
            }
                
            j = 0;
            newestNoteDuration = 0;
            for (j = 0; j < 16; j++){
               if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
                  newestNote = heldNoteNumberArray[j];
               }
            }
            insidePreviousMidiNoteNumber = insideMidiNoteNumber;
            insideMidiNoteNumber = heldNoteNumberArray[j];
            insidePreviousPitchInMicros = insidePitchInMicros;
            insideTargetPitchInMicros = outsidePitchInMicros;
            portamentoRunning = HIGH;
            insideRun = HIGH;
         }
              
         if (portamentoRunning == HIGH){
            if (insidePreviousPitchInMicros < insideTargetPitchInMicros) {
               portamentoDirection = LOW; 
               //Going down in pitch.  insidePitchInMicros will grow.
            }
            else {
               portamentoDirection = HIGH; 
               //Going up in pitch.  insidePitchInMicros will shrink.
            }
         }
              
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2( 
            insidePitchInMicros, insideBendFactor, vibFactor, 
            insidePitfall, pitfallTuningOffset);
            
         ch2_Arpeggiator(9,0);
                                           
      break;


      // Note Off
      case 3:
         if (outsideMidiNoteNumber > 0){
         
            // If the arpeggiator is turned off OR there are two notes held 
            // down by the arpeggiator.
            j = 0;
            highestNoteNumber = 0;
            for (j = 0; j < 16; j++) {
               if (heldNoteNumberArray[j] == outsideMidiNoteNumber) {
                  heldNoteNumberDurationMillis[j] = 0;
                  heldNoteNumberArray[j] = 0;
               }
               if (heldNoteNumberArray[j] > 0) {
                  highestNoteNumber = heldNoteNumberArray[j];
               }  
            }
            if (highestNoteNumber == 0) {
               insideRun = LOW;
               digitalWriteFast(audioTwo, LOW);
               insidePreviousMidiNoteNumber = 0;
            }
         else {
            j = 0;
            newestNoteDuration = 0;
            for (j = 0; j < 16; j++) {
               if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
               newestNote = heldNoteNumberArray[j];
               }
            }
            if ((ch2_Arpeggiator(5,0) == LOW) || ((ch2_Arpeggiator(5,0) == 
               HIGH) && (ch2_Arpeggiator(7,0) == HIGH) && 
               (ch2_Arpeggiator(8,0) == LOW))) {

               OnNoteOn(2,newestNote,127);
            }
         }
         
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2( 
            insidePitchInMicros, insideBendFactor, vibFactor, 
            insidePitfall, pitfallTuningOffset);
         ch2_Arpeggiator(9,testArrayContentsForTwoNotesOrMore(
            heldNoteNumberArray));
         }
                   
      break;


      // Pitch Bend
      case 4: 
         insideBendFactor = outsideBendFactor;
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2( 
               insidePitchInMicros, insideBendFactor, vibFactor, 
               insidePitfall, pitfallTuningOffset);
      break;

      // CC 1 Change
      case 5: 
         insideCC1 = outsideCC;
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(
            insidePitchInMicros, insideBendFactor, vibFactor, 
            insidePitfall, pitfallTuningOffset);
                
      break;
      
      // CC 2 Change (Reserved for Bend Range)
      case 6:
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(
            insidePitchInMicros, insideBendFactor, vibFactor, 
            insidePitfall, pitfallTuningOffset);
      break;
      
      // CC 3 Change (vibrato type)
      case 7:
         insideCC3 = outsideCC;
         if (insideCC3 > 0) {
            vibWaveform = 1;
         }
         else {
            vibWaveform = 0;
         }
        
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(
            insidePitchInMicros, insideBendFactor, vibFactor, 
            insidePitfall, pitfallTuningOffset);
              
      break;
      
      // CC 4 Change (Reserved for Arpeggiator On)
      case 8:
      break;
      
      // CC 6 Change (Polypulse Activate)
      case 9: 
         insideCC5 = outsideCC;
         if (insideCC5 > 0) {
            insidePitfall = HIGH;
         }
         else {
            insidePitfall = LOW;
         }
        
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(
            insidePitchInMicros, insideBendFactor, vibFactor, 
            insidePitfall, pitfallTuningOffset);
        
      break;
      
      // Returns newest note to Arpeggiator when it turns off.
      case 10:
         j = 0;
         newestNoteDuration = 0;
         for (j = 0; j < 16; j++) {
            if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
               newestNote = heldNoteNumberArray[j];
               newestNoteDuration = heldNoteNumberDurationMillis[j];
            }      
         }
        
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(
            insidePitchInMicros, insideBendFactor, vibFactor, 
            insidePitfall, pitfallTuningOffset);
              
         return newestNote;
      break;
      
      case 11: //Returns insideDecay on other three oscillators
         return LOW;
      break;

      case 12:
         portamentoSpeedInMicros = map(outsideCC,0,127,20,2000);
      break;
   }
}



// CHANNEL 3: 25% Pulse Nintendo Sound Oscillator
int ch3_Oscillator(byte dataType, byte outsideMidiNoteNumber, 
   float outsidePitchInMicros, float outsideBendFactor, byte outsideCC) {

   static boolean insideRun = LOW;
   static byte insideMidiNoteNumber = 0;
   static unsigned long insidePitchInMicros = 0;
   static unsigned long insidePitchInMicrosAfterBendFactor = 0;
   static float insideBendFactor = 1.0;
   static byte insideCC1 = 0;
   static byte insideCC3 = 0;
   static byte insideCC5 = 0;
   static boolean insideHigh = LOW;
   static unsigned long insidePreviousMicros = 0;
   unsigned long currentMicros;
   static byte dutyCycleOffFactor = 150;
   static byte dutyCycleOnFactor = 50;
   static byte insidePreviousMidiNoteNumber;
   static boolean portamentoRunning;
   static boolean portamentoDirection;
   static float portamentoSpeed = 1.0;
   static float portamentoSpeedInMicros = 100.0 / (float)portamentoSpeed;
   static unsigned long previousPortamentoChange;
   static unsigned long insidePreviousPitchInMicros = 0;
   static unsigned long insideTargetPitchInMicros = 0;
   static unsigned long insidePreviousDecayMicros = 0;
   static boolean insideDecay = LOW;
   static unsigned int insideDecayInterval;
   static byte heldNoteNumberArray[16];
   static unsigned long heldNoteNumberDurationMillis[16];
   static byte j = 0;
   static boolean blankSlotFound = LOW;
   static unsigned long newestNoteDuration = 0;
   static byte highestNoteNumber = 0;
   static byte newestNote = 0;
   static float portamentoSpeedConstant = 0.0018;
   static byte defaultDutyCycleOff = 150;
   static byte defaultDutyCycleOn = 50;
   unsigned long elapsedTimeInMicros;
   static unsigned long finalPitchInMicrosDutyOff;
   static unsigned long finalPitchInMicrosDutyOn;
   float portamentoStepInMicros;

  switch(dataType){
      // Run Current Oscillator Settings
      case 1:  
         if (insideRun == HIGH){
            // Oscillator guts
            currentMicros = micros();
                    
            // If it's time to decrease the pitch, do so, but 
            // scale it non-linearly so that the speed is always 
            // the same as if the shift is passing through 500 Hz. 
            // If the pitch is higher than 500 Hz, slow the portamento 
            // down.  If the pitch is lower than 500 Hz, speed the 
            // portamento up. 
            if ((portamentoRunning == HIGH) && (insideDecay == LOW) && 
               (portamentoDirection == LOW) && 
               ((currentMicros - previousPortamentoChange) >= 
               portamentoSpeedInMicros)) {
               
               portamentoStepInMicros = (portamentoSpeedConstant / 
                  (1.0 / (float)insidePitchInMicros));
                  
               if (portamentoStepInMicros < 1.0) {
                  portamentoStepInMicros = 1;
               }
               insidePitchInMicros = insidePitchInMicros + 
                  portamentoStepInMicros;
               previousPortamentoChange = micros();
                   
               insidePitchInMicrosAfterBendFactor = (insidePitchInMicros
                  * (float)insideBendFactor);
               finalPitchInMicrosDutyOff = 
                  insidePitchInMicrosAfterBendFactor * 
                  (float)dutyCycleOffFactor/100.0;
               finalPitchInMicrosDutyOn = 
                  insidePitchInMicrosAfterBendFactor * 
                  (float)dutyCycleOnFactor/100.0;
	             
               if (insidePitchInMicros >= 
                  insideTargetPitchInMicros) {
                     
                  portamentoRunning = LOW;
                  insidePitchInMicros = 
                     insideTargetPitchInMicros;
               }
            }

            // If it's time to increase the pitch, do so, but scale it 
            // non-linearly so that the speed is always the same as if 
            // the shift is passing through 500 Hz.  If the pitch is 
            // higher than 500 Hz, slow the portamento down.  If the 
            // pitch is lower than 500 Hz, speed the portamento up.
            if ((portamentoRunning == HIGH) && (insideDecay == LOW) && 
               (portamentoDirection == HIGH) && ((currentMicros - 
               previousPortamentoChange) >= portamentoSpeedInMicros)) {
	              portamentoStepInMicros = (portamentoSpeedConstant / 
                  (1.0 / (float)insidePitchInMicros));
                    
               if (portamentoStepInMicros < 1.0) {
                  portamentoStepInMicros = 1;
               }
               insidePitchInMicros = insidePitchInMicros - 
                  portamentoStepInMicros;
               previousPortamentoChange = micros(); 
               insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
                  (float)insideBendFactor);
               finalPitchInMicrosDutyOff = 
                  insidePitchInMicrosAfterBendFactor * 
                  (float)dutyCycleOffFactor/100.0;
               finalPitchInMicrosDutyOn = 
                  insidePitchInMicrosAfterBendFactor * 
                  (float)dutyCycleOnFactor/100.0;
            
               if (insidePitchInMicros <= insideTargetPitchInMicros) {
                  portamentoRunning = LOW;
                  insidePitchInMicros = insideTargetPitchInMicros;
               }
            }
               
               
            if ((portamentoRunning == HIGH) && (insideDecay == HIGH)) {
               insidePitchInMicros = insideTargetPitchInMicros;
               portamentoRunning = LOW;
                 
               insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
                  (float)insideBendFactor);
               finalPitchInMicrosDutyOff = 
                  insidePitchInMicrosAfterBendFactor * 
                  (float)dutyCycleOffFactor/100.0;
               finalPitchInMicrosDutyOn = 
                  insidePitchInMicrosAfterBendFactor * 
                  (float)dutyCycleOnFactor/100.0;
            }
               
            currentMicros = micros();
            elapsedTimeInMicros = currentMicros - insidePreviousMicros;
            if ((insideHigh == LOW) && (elapsedTimeInMicros >= 
               finalPitchInMicrosDutyOff) && (dutyCycleOffFactor < 200)) {
               
               digitalWriteFast(audioThree,HIGH);
               insideHigh = HIGH;
               insidePreviousMicros = micros(); 
            }
            else if ((insideHigh == HIGH) && (elapsedTimeInMicros >= 
               finalPitchInMicrosDutyOn) && (dutyCycleOffFactor < 200)) {
               
               digitalWriteFast(audioThree,LOW);
               insideHigh = LOW;
               insidePreviousMicros = micros(); 
            }
               
            currentMicros = micros();
            if ( ((currentMicros - (float)insidePreviousDecayMicros) >= 
               insideDecayInterval) && (insideDecay == HIGH) && 
               (dutyCycleOffFactor < 200) && (dutyCycleOnFactor > 0)) {
                 
               dutyCycleOffFactor ++;
               dutyCycleOnFactor --;
               insidePreviousDecayMicros = micros();
                 
               finalPitchInMicrosDutyOff = 
                  insidePitchInMicrosAfterBendFactor * 
                  (float)dutyCycleOffFactor/100.0;
               finalPitchInMicrosDutyOn = 
                  insidePitchInMicrosAfterBendFactor * 
                  (float)dutyCycleOnFactor/100.0;
            }
               
            return insideMidiNoteNumber;  
         }
            
         if (insideRun == LOW) {
            return 0;
         }
      break;

      // Note On
      case 2: 
         if ((insideRun == LOW) && (outsideMidiNoteNumber > 0) && 
            (ch3_Arpeggiator(5,0) == LOW)) {
                
            portamentoRunning = LOW;
            insidePitchInMicros = outsidePitchInMicros;
            insidePreviousMidiNoteNumber = 0;
            insideRun = HIGH;
            insideMidiNoteNumber = outsideMidiNoteNumber;
            
            if (isThisNoteInArray(heldNoteNumberArray,outsideMidiNoteNumber) ==
               LOW){
            
               j = 0;
               blankSlotFound = LOW;
               while (blankSlotFound == LOW) {
                  if ((heldNoteNumberArray[j] != 0) && (j < 16)) {
                     j++;
                  }
                  else {
                     blankSlotFound = HIGH;
                  }
               }
               heldNoteNumberArray[j] = outsideMidiNoteNumber;
               heldNoteNumberDurationMillis[j] = millis();
            }
         }
              
         if ((ch3_Arpeggiator(5,0) == HIGH) && (outsideMidiNoteNumber > 0)) {
            portamentoRunning = LOW;
            insidePitchInMicros = outsidePitchInMicros;
            insidePreviousMidiNoteNumber = 0;
            insideRun = HIGH;
            insideMidiNoteNumber = outsideMidiNoteNumber;
         }
              
         if ((insideRun == HIGH) && (ch3_Arpeggiator(5,0) == LOW) && 
            (outsideMidiNoteNumber > 0)) {
                
            if (isThisNoteInArray(heldNoteNumberArray,outsideMidiNoteNumber) == 
               LOW) {
                
               j = 0;
               blankSlotFound = LOW;
               while (blankSlotFound == LOW) {
                  if ((heldNoteNumberArray[j] != 0) && (j < 16)) {
                     j++;
                  }
                  else {
                     blankSlotFound = HIGH;
                  }
               }
               heldNoteNumberArray[j] = outsideMidiNoteNumber;
               heldNoteNumberDurationMillis[j] = millis();
            }
                
            j = 0;
            newestNoteDuration = 0;
            for (j = 0; j < 16; j++) {
               if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
                  newestNote = heldNoteNumberArray[j];
               }
            }
            insidePreviousMidiNoteNumber = insideMidiNoteNumber;
            insideMidiNoteNumber = heldNoteNumberArray[j];
            insidePreviousPitchInMicros = insidePitchInMicros;
            insideTargetPitchInMicros = outsidePitchInMicros;
            portamentoRunning = HIGH;
            insideRun = HIGH;
         }
              
         if (insideDecay == HIGH) {
            insideCC1 = 0;
            dutyCycleOffFactor = 150;
            dutyCycleOnFactor = 50;
         }
              
         if (portamentoRunning == HIGH) {
            if (insidePreviousPitchInMicros < insideTargetPitchInMicros) {
               //Going down in pitch.  insidePitchInMicros will grow.
               portamentoDirection = LOW; 
            }
            else {
               portamentoDirection = HIGH; //Going up in pitch.  insidePitchInMicros will shrink.
            }
         }
              
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * (float)dutyCycleOnFactor/100.0;
         
         ch3_Arpeggiator(9,0);
      
      break;
      
      // Note Off
      case 3:
         if (outsideMidiNoteNumber > 0) {
            j = 0;
            highestNoteNumber = 0;
            for (j = 0; j < 16; j++) {
               if (heldNoteNumberArray[j] == outsideMidiNoteNumber) {
                  heldNoteNumberDurationMillis[j] = 0;
                  heldNoteNumberArray[j] = 0;
               }
               if (heldNoteNumberArray[j] > 0) {
                  highestNoteNumber = heldNoteNumberArray[j];
               }  
            }
            if (highestNoteNumber == 0) {
               insideRun = LOW;
               digitalWriteFast(audioThree, LOW);
               insidePreviousMidiNoteNumber = 0;
            }
            else if (insideDecay == LOW) {
               j = 0;
               newestNoteDuration = 0;
               for (j = 0; j < 16; j++) {
                  if ((heldNoteNumberDurationMillis[j] > newestNoteDuration) && 
                     (insideDecay == LOW)) {
                     
                     newestNote = heldNoteNumberArray[j];
                  }
               }
               
               if ((ch3_Arpeggiator(5,0) == LOW) || ((ch3_Arpeggiator(5,0) == 
                  HIGH) && (ch3_Arpeggiator(7,0) == HIGH) && 
                  (ch3_Arpeggiator(8,0) == LOW))) {
	     
                  OnNoteOn(3,newestNote,127);
               }
            }
          
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
          
         ch3_Arpeggiator(9,
            testArrayContentsForTwoNotesOrMore(heldNoteNumberArray));
         }
      break;

      // Pitch Bend
      case 4: 
         insideBendFactor = outsideBendFactor;
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
      break;
      
      // CC 1 Change
      case 5: 
         insideCC1 = outsideCC;
         dutyCycleOffFactor = map(insideCC1, 0, 127, defaultDutyCycleOff, 200);
         dutyCycleOnFactor = map(insideCC1, 0, 127, defaultDutyCycleOn, 0);
              
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
      break;
       
      // CC 2 Change
      case 6:
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
      break;

      // CC 3 Change
      case 7: insideCC3 = outsideCC;
         if (insideCC3 > 0){
            insideDecay = HIGH;
            insideDecayInterval = map(insideCC3,0,127,10000,500);
         }
         else {
            insideDecay = LOW;
            ch3_Oscillator(5,0,0,0,0);
         }
        
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
      break;
      
      // CC 4 Change
      case 8:
      break;
      
      case 9: // CC 6 Change
         insideCC5 = outsideCC;
         defaultDutyCycleOff = map(insideCC5, 0, 127, 150, 100);
         defaultDutyCycleOn = map(insideCC5, 0, 127, 50, 100);
         dutyCycleOffFactor = map(insideCC1, 0, 127, defaultDutyCycleOff, 200);
         dutyCycleOnFactor = map(insideCC1, 0, 127, defaultDutyCycleOn, 0);
         
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
      break;
      
      // Returns newest note to Arpeggiator when it turns off.
      case 10:
         j = 0;
         newestNoteDuration = 0;
         for (j = 0; j < 16; j++) {
            if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
               newestNote = heldNoteNumberArray[j];
               newestNoteDuration = heldNoteNumberDurationMillis[j];
            }      
         }
        
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
              
         return newestNote;
      break;
      
      case 11:
         return insideDecay;
      break;

      case 12:
         portamentoSpeedInMicros = map(outsideCC,0,127,20,2000);
      break;
   }
}






// CHANNEL 4: 12.5% Pulse Nintendo Sound Oscillator
int ch4_Oscillator(byte dataType, byte outsideMidiNoteNumber, 
   float outsidePitchInMicros, float outsideBendFactor, byte outsideCC) {
   
   static boolean insideRun = LOW;
   static byte insideMidiNoteNumber = 0;
   static unsigned long insidePitchInMicros = 0;
   static unsigned long insidePitchInMicrosAfterBendFactor = 0;
   static float insideBendFactor = 1.0;
   static byte insideCC1 = 0;
   static byte insideCC3 = 0;
   static byte insideCC5 = 0;
   static boolean insideHigh = LOW;
   static unsigned long insidePreviousMicros = 0;
   unsigned long currentMicros;
   static byte dutyCycleOffFactor = 175;
   static byte dutyCycleOnFactor = 25;
   static byte insidePreviousMidiNoteNumber;
   static boolean portamentoRunning;
   static boolean portamentoDirection;
   static float portamentoSpeed = 1.0;
   static float portamentoSpeedInMicros = 100.0 / (float)portamentoSpeed;
   static unsigned long previousPortamentoChange;
   static unsigned long insidePreviousPitchInMicros = 0;
   static unsigned long insideTargetPitchInMicros = 0;
   static unsigned long insidePreviousDecayMicros = 0;
   static boolean insideDecay = LOW;
   static unsigned int insideDecayInterval;
   static byte heldNoteNumberArray[16];
   static unsigned long heldNoteNumberDurationMillis[16];
   static byte j = 0;
   static boolean blankSlotFound = LOW;
   static unsigned long newestNoteDuration = 0;
   static byte highestNoteNumber = 0;
   static byte newestNote = 0;
   static float portamentoSpeedConstant = 0.0018;
   static byte defaultDutyCycleOff = 175;
   static byte defaultDutyCycleOn = 25;
   unsigned long elapsedTimeInMicros;
   static unsigned long finalPitchInMicrosDutyOff;
   static unsigned long finalPitchInMicrosDutyOn;
   float portamentoStepInMicros;

   switch(dataType) {
      // Run Current Oscillator Settings
      case 1:  
         if (insideRun == HIGH) {
            // Oscillator guts
            currentMicros = micros();
            // If it's time to decrease the pitch, do so, but scale it 
            // non-linearly so that the speed is always the same as if 
            // the shift is passing through 500 Hz.  If the pitch is higher 
            // than 500 Hz, slow the portamento down.  If the pitch is lower 
            // than 500 Hz, speed the portamento up.
            if ((portamentoRunning == HIGH) && (insideDecay == LOW) && 
               (portamentoDirection == LOW) && 
               ((currentMicros - previousPortamentoChange) >= 
               portamentoSpeedInMicros)) {

	            portamentoStepInMicros = (portamentoSpeedConstant / (1.0 / 
                  (float)insidePitchInMicros));
               
               if (portamentoStepInMicros < 1.0) {
                  portamentoStepInMicros = 1;
               }
               
               insidePitchInMicros = insidePitchInMicros + 
                  portamentoStepInMicros;
               
               previousPortamentoChange = micros();
                     
               insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
                  (float)insideBendFactor);
               finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor *
                  (float)dutyCycleOffFactor/100.0;
               finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor *
                  (float)dutyCycleOnFactor/100.0;     
	             
                    
               if (insidePitchInMicros >= insideTargetPitchInMicros) {
                  portamentoRunning = LOW;
                  insidePitchInMicros = insideTargetPitchInMicros;
               }
            }
            
            // If it's time to increase the pitch, do so, but scale it 
            // non-linearly so that the speed is always the same as if 
            // the shift is passing through 500 Hz.  If the pitch is 
            // higher than 500 Hz, slow the portamento down.  If the 
            // pitch is lower than 500 Hz, speed the portamento up.

            if ((portamentoRunning == HIGH) && (insideDecay == LOW) && 
               (portamentoDirection == HIGH) && ((currentMicros - 
               previousPortamentoChange) >= portamentoSpeedInMicros)) {
	             
               portamentoStepInMicros = (portamentoSpeedConstant / 
                  (1.0 / (float)insidePitchInMicros));
                     
               if (portamentoStepInMicros < 1.0) {
                  portamentoStepInMicros = 1;
               }
               
               insidePitchInMicros = insidePitchInMicros - 
                  portamentoStepInMicros;
               
               previousPortamentoChange = micros();
                     
               insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
                  (float)insideBendFactor);
               finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor *
                  (float)dutyCycleOffFactor/100.0;
               finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor *
                  (float)dutyCycleOnFactor/100.0; 
	             
               if (insidePitchInMicros <= insideTargetPitchInMicros) {
                  portamentoRunning = LOW;
                  insidePitchInMicros = insideTargetPitchInMicros;
               }
            }
                  
                  
            if ((portamentoRunning == HIGH) && (insideDecay == HIGH)) {
               insidePitchInMicros = insideTargetPitchInMicros;
               portamentoRunning = LOW;
                    
               insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
                  (float)insideBendFactor);
               finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor *
                  (float)dutyCycleOffFactor/100.0;
               finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
                  (float)dutyCycleOnFactor/100.0;
            }
                  
                  
            currentMicros = micros();
            elapsedTimeInMicros = currentMicros - insidePreviousMicros;
            if ((insideHigh == LOW) && (elapsedTimeInMicros >= 
               finalPitchInMicrosDutyOff) && (dutyCycleOffFactor < 200)) {
               
               digitalWriteFast(audioFour, HIGH);
               insideHigh = HIGH;
               insidePreviousMicros = micros(); 
            }
                  
            else if ((insideHigh == HIGH) && (elapsedTimeInMicros >= 
               finalPitchInMicrosDutyOn) && (dutyCycleOffFactor < 200)) {
               
               digitalWriteFast(audioFour,LOW);
               insideHigh = LOW;
               insidePreviousMicros = micros(); 
            }
                  
            currentMicros = micros();
            if ( ((currentMicros - insidePreviousDecayMicros) >= 
               insideDecayInterval) && (insideDecay == HIGH) && 
               (dutyCycleOffFactor < 200) && (dutyCycleOnFactor > 0)) {

               dutyCycleOffFactor ++;
               dutyCycleOnFactor --;
               insidePreviousDecayMicros = micros();
                    
               finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor *
                  (float)dutyCycleOffFactor/100.0;
               finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor *
                  (float)dutyCycleOnFactor/100.0;
            }
            
            return insideMidiNoteNumber;  
         }
            
         if (insideRun == LOW) {
            return 0;
         }
      break;
      
      // Note On
      case 2: 
         if ((insideRun == LOW) && (outsideMidiNoteNumber > 0) && 
            (ch4_Arpeggiator(5,0) == LOW)) {
            
            portamentoRunning = LOW;
            insidePitchInMicros = outsidePitchInMicros;
            insidePreviousMidiNoteNumber = 0;
            insideRun = HIGH;
            insideMidiNoteNumber = outsideMidiNoteNumber;
            
            if (isThisNoteInArray(heldNoteNumberArray,outsideMidiNoteNumber) == 
               LOW) {
            
               j = 0;
               blankSlotFound = LOW;
               while (blankSlotFound == LOW) {
                  if ((heldNoteNumberArray[j] != 0) && (j < 16)) {
                     j++;
                  }
                  else {
                     blankSlotFound = HIGH;
                  }
               }
               heldNoteNumberArray[j] = outsideMidiNoteNumber;
               heldNoteNumberDurationMillis[j] = millis();
            }
         }
              
         if ((ch4_Arpeggiator(5,0) == HIGH) && (outsideMidiNoteNumber > 0)) {
            portamentoRunning = LOW;
            insidePitchInMicros = outsidePitchInMicros;
            insidePreviousMidiNoteNumber = 0;
            insideRun = HIGH;
            insideMidiNoteNumber = outsideMidiNoteNumber;
         }
              
         if ((insideRun == HIGH) && (ch4_Arpeggiator(5,0) == LOW) && 
            (outsideMidiNoteNumber > 0)) {
                
            if (isThisNoteInArray(heldNoteNumberArray,outsideMidiNoteNumber) ==
               LOW) {
                
               j = 0;
               blankSlotFound = LOW;
               while (blankSlotFound == LOW) {
                  if ((heldNoteNumberArray[j] != 0) && (j < 16)) {
                     j++;
                  }
                  else {
                     blankSlotFound = HIGH;
                  }
               }
               heldNoteNumberArray[j] = outsideMidiNoteNumber;
               heldNoteNumberDurationMillis[j] = millis();
            }
                
            j = 0;
            newestNoteDuration = 0;
            for (j = 0; j < 16; j++) {
               if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
                  newestNote = heldNoteNumberArray[j];
               }
            }
            insidePreviousMidiNoteNumber = insideMidiNoteNumber;
            insideMidiNoteNumber = heldNoteNumberArray[j];
            insidePreviousPitchInMicros = insidePitchInMicros;
            insideTargetPitchInMicros = outsidePitchInMicros;
            portamentoRunning = HIGH;
            insideRun = HIGH;
         }
              
         if (insideDecay == HIGH) {
            insideCC1 = 0;
            dutyCycleOffFactor = 150;
            dutyCycleOnFactor = 50;
         }
              
         if (portamentoRunning == HIGH) {
            if (insidePreviousPitchInMicros < insideTargetPitchInMicros) {
               //Going down in pitch.  insidePitchInMicros will grow.
               portamentoDirection = LOW; 
            }
            else {
               //Going up in pitch.  insidePitchInMicros will shrink.
               portamentoDirection = HIGH; 
            }
         }
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
         
         ch4_Arpeggiator(9,0);
      
      break;
      
      // Note Off
      case 3:
         if (outsideMidiNoteNumber > 0) { 
            j = 0;
            highestNoteNumber = 0;
            for (j = 0; j < 16; j++) {
               if (heldNoteNumberArray[j] == outsideMidiNoteNumber) {
                  heldNoteNumberDurationMillis[j] = 0;
                  heldNoteNumberArray[j] = 0;
               }
               if (heldNoteNumberArray[j] > 0) {
                  highestNoteNumber = heldNoteNumberArray[j];
               }  
            }
            if (highestNoteNumber == 0) {
               insideRun = LOW;
               digitalWriteFast(audioFour, LOW);
               insidePreviousMidiNoteNumber = 0;
            }
            else if (insideDecay == LOW) {
               j = 0;
               newestNoteDuration = 0;
               for (j = 0; j < 16; j++) {
                  if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
                     newestNote = heldNoteNumberArray[j];
                  }
               }
               if ((ch4_Arpeggiator(5,0) == LOW) || ((ch4_Arpeggiator(5,0) == 
                  HIGH) && (ch4_Arpeggiator(7,0) == HIGH) && 
                  (ch4_Arpeggiator(8,0) == LOW))) {
               
                  OnNoteOn(4,newestNote,127);
               }
            }
         }
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
         
         ch4_Arpeggiator(9,
            testArrayContentsForTwoNotesOrMore(heldNoteNumberArray));
      break;

      // Pitch Bend
      case 4: 
         insideBendFactor = outsideBendFactor;
                  // Oscillator guts
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;       
      break;
      
      // CC 1 Change
      case 5: 
         insideCC1 = outsideCC;
         dutyCycleOffFactor = map(insideCC1, 0, 127, defaultDutyCycleOff, 200);
         dutyCycleOnFactor = map(insideCC1, 0, 127, defaultDutyCycleOn, 0);
         
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;       
      break;
          
      // CC 2 Change
      case 6:
          insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
          finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
          finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
      break;

      // CC 3 Change
      case 7: insideCC3 = outsideCC;
         if (insideCC3 > 0) {
            insideDecay = HIGH;
            insideDecayInterval = map(insideCC3,0,127,10000,500);
         }
         else {
            insideDecay = LOW;
            ch4_Oscillator(5,0,0,0,0);
         }
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
      break;
      
      // CC 4 Change
      case 8:
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
      break;
      
      case 9: // CC 6 Change
         insideCC5 = outsideCC;
         defaultDutyCycleOff = map(insideCC5, 0, 127, 175, 100);
         defaultDutyCycleOn = map(insideCC5, 0, 127, 25, 100);
         dutyCycleOffFactor = map(insideCC1, 0, 127, defaultDutyCycleOff, 200);
         dutyCycleOnFactor = map(insideCC1, 0, 127, defaultDutyCycleOn, 0);
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
      break;
      
      // Returns newest note to Arpeggiator when it turns off.
      case 10:
         j = 0;
         newestNoteDuration = 0;
         for (j = 0; j < 16; j++) {
            if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
               newestNote = heldNoteNumberArray[j];
               newestNoteDuration = heldNoteNumberDurationMillis[j];
            }      
         }
         insidePitchInMicrosAfterBendFactor = (insidePitchInMicros * 
            (float)insideBendFactor);
         finalPitchInMicrosDutyOff = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOffFactor/100.0;
         finalPitchInMicrosDutyOn = insidePitchInMicrosAfterBendFactor * 
            (float)dutyCycleOnFactor/100.0;
         return newestNote;
      break;
      
      case 11:
         return insideDecay;
      break;

      case 12:
         portamentoSpeedInMicros = map(outsideCC,0,127,20,2000);
      break;
   }
}




void mainSampleEngine(byte dataType, byte noteNumber, byte noteVelocity, 
   long longData) {
   
   static boolean insideRun = LOW;
   static unsigned long sampleCounter = 0;
   static byte sampleCounterIncrement = 1;
   static unsigned long maxSampleCounter = 400;
   byte current8bitSampleLevel;
   static byte sampleNumber;
   static byte insideNoteNumber;
   unsigned long currentMicros;
   static unsigned long previousMicros;
   unsigned long elapsedTimeInMicros;
   static unsigned long sampleRateMicros = 63;
   static byte sampleIncrementRate = 1;

   switch (dataType) {
   
      case 1: // Run in main loop
         if (insideRun == HIGH) {
            if (sampleCounter >= maxSampleCounter) {
               sampleCounter = 0;
               insideRun = LOW;
            }
         }
     
         if (insideRun == HIGH) {
            currentMicros = micros();
            elapsedTimeInMicros = currentMicros - previousMicros;
            if (elapsedTimeInMicros >= sampleRateMicros) {
               current8bitSampleLevel = pgm_read_byte_near(sampleData + 
                  (sampleStarts[sampleNumber]) + sampleCounter);
               analogWrite(A14,current8bitSampleLevel);
               sampleCounter = sampleCounter + sampleIncrementRate;
               previousMicros = micros();
            }
         }
      break;
   
      case 2: // Note On
         if ((insideRun == LOW) || (insideRun == HIGH)) {
            insideNoteNumber = noteNumber;
            if ((noteNumber >= 48) && (noteNumber <= 66)) {
               sampleNumber = noteNumber - 48;
       
               sampleCounter = 0;
               maxSampleCounter = sampleStarts[sampleNumber + 1] - 
                  sampleStarts[sampleNumber];
       
               insideRun = HIGH;
            }
         }
     
   break;
   
   case 3: // Note Off
      if (insideNoteNumber == noteNumber) {
         insideRun = LOW;
         analogWrite(A14,127);
      }
     
   break;
   
   case 4: // Pitch Bend
      if (longData == 8192) {
         sampleRateMicros = 60;
      }

      if (longData <= 8192) {
         sampleRateMicros = map(longData,0,8191,40,60);
      }
      if (longData >= 8193) {
         sampleRateMicros = map(longData,8193,16838,60,30);
      }

   break;
   
   case 5: // CC1 Change
      if (noteNumber > 0) {
         sampleRateMicros = 60 * pow(20,((float)noteNumber/127));
      }
      else {
         sampleRateMicros = 60;
      }
   break;
       
   }
}



int mainWaveformEngine(byte dataType, byte outsideMidiNoteNumber, 
   float outsidePitchInMicros, float outsideBendFactor, byte outsideCC) {

   static boolean insideRun = LOW;
   static byte insideMidiNoteNumber = 0;
   static unsigned long insidePitchInMicros = 0;
   static unsigned long insidePitchInMicrosAfterBendFactor = 0;
   static float insideBendFactor = 1.0;
   static byte insideCC1 = 0;
   static byte insideCC3 = 0;
   static byte insideCC5 = 0;
   static boolean insideHigh = LOW;
   static unsigned long insidePreviousMicros = 0;
   unsigned long currentMicros;
   static byte insidePreviousMidiNoteNumber;
   static boolean portamentoRunning;
   static boolean portamentoDirection;
   static float portamentoSpeed = 1.0;
   static float portamentoSpeedInMicros = 100.0 / (float)portamentoSpeed;
   static unsigned long previousPortamentoChange;
   static unsigned long insidePreviousPitchInMicros = 0;
   static unsigned long insideTargetPitchInMicros = 0;
   static boolean insidePitfall = LOW;
   static int insidePitfallPosition = 0;
   // static int insidePitfallWaveform = B0011101111100110;
   static int insidePitfallWaveform = 0x3BE6;
   static unsigned long insidePitchInMicros2 = 0;
   static byte heldNoteNumberArray[16];
   static unsigned long heldNoteNumberDurationMillis[16];
   static byte j = 0;
   static boolean blankSlotFound = LOW;
   static unsigned long newestNoteDuration = 0;
   static byte highestNoteNumber = 0;
   static byte newestNote = 0;
   static float portamentoSpeedConstant = 0.0018;
   unsigned long currentMillis;
   unsigned long elapsedTimeInMicros;
   
   //Lower the number means lower the pitch of the pitfall polypulse sound.
   static float pitfallTuningOffset = 0.87; 
   
   float portamentoStepInMicros;
              
   // Vibrato variables
   static unsigned long previousVibCountMicros = 0;
   static int vibCount = 0;
   static boolean vibDirection = 0; //0 counts up, 1 counts down
   static int vibAmount = 0;
   static float vibAmount2 = 0.0;
   static float vibFactor = 1.0;
   static int modWheelFactor = 0;
   static float finalFrequency = 0;
   unsigned long currentVibCountMicros;

   // Resolution of the sine vibrato.  (Calculates every x microseconds)
   static unsigned long sineVibSpeedStepsInMicros = 2000;

   // 0 is triangle vibrato.  1 is sine vibrato. 
   static byte vibWaveform = 1;

   // Reciprocal of Vib Speed for Triangle Vibrato 
   // (smaller number means faster)
   static unsigned long triangleVibSpeedStepsInMicros = 320; 
   
   static float vibRateDivisor = 23.0;
   
   static byte waveform1[32] = {255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,
      255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0};
   static byte waveform2[32] = {255,255,255,0,0,0,0,0,255,255,255,0,0,0,0,0,255,
      255,255,0,0,0,0,0,255,255,255,0,0,0,0,0};
   static byte waveform3[32] = {255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,255,255,
      0,0,0,0,0,0,255,255,0,0,0,0,0,0};
   static byte waveform4[32] = {255,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,255,0,0,0,0,0,
      0,0,255,0,0,0,0,0,0,0};
   static byte waveform5[32] = {127,255,127,0,127,255,127,0,127,255,127,0,127,255,
      127,0,127,255,127,0,127,255,127,0,127,255,127,0,127,255,80,112};
   static byte waveform6[32] = {0,80,160,255,0,80,160,255,0,80,160,255,0,80,160,255,
      0,80,160,255,0,80,160,255,0,80,160,255,0,80,160,255};            
   static byte waveform7[32] = {127,191,255,191,127,63,0,63,127,191,255,191,127,63,0,
      63,127,191,255,191,127,63,0,63,127,191,255,191,127,63,0,63};
   static byte waveform8[32] = {255,255,255,0,255,255,255,255,0,255,0,255,0,0,255,0,
      255,255,255,0,255,255,255,255,0,255,0,255,0,0,255,0};
   static byte waveform9[32] = {255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,
      255,0,0,255,255,0,0,255,255,0,0,255,255,0,0};
   static byte waveform10[32] = {255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,255,
      0,255,0,255,0,255,0,255,0,255,0,255,0,255,0};
   static byte waveform11[32] = {255,255,255,255,0,0,0,0,127,255,127,255,0,0,0,0,255,
      0,127,0,255,0,127,0,255,0,127,0,255,0,127,0};
   static byte waveform12[32] = {127,15,255,192,111,127,221,209,255,145,101,139,15,
      209,0,25,133,127,209,25,127,209,219,209,127,34,0,219,34,127,241,34};
   
   static byte sampleNumber = 1;
   static float sampleRateMicros;
   static byte sampleIncrementRate = 1;
   static unsigned long sampleCounter = 0;
   byte current8bitSampleLevel;


  switch(dataType){
                
      // Run Current Oscillator Settings
      case 1:  
         if (insideRun == HIGH) {
         
            // TRIANGLE VIBRATO - creating vibAmount
            if (vibWaveform == 0) {
               if (insideCC1 > 0) {
                  if (vibCount <= 0) { // Set direction of count
                     vibDirection = 0; 
                  }
                  if (vibCount >= 200) {
                     vibDirection = 1; 
                  }
                
                  currentVibCountMicros = micros();
              
                  if ((vibDirection == 0) && ((currentVibCountMicros - 
                     previousVibCountMicros) >= 
                     ((float)triangleVibSpeedStepsInMicros / 
                     (float)vibRateFactor))) {
                     
                     // If the proper amount of microseconds have passed 
                     // and we're counting up, count up one.
                     vibCount ++; 

                     // Set the current time for reference the next time.
                     previousVibCountMicros = micros(); 
                  }
              
                  if ((vibDirection == 1) && ((currentVibCountMicros - 
                     previousVibCountMicros) >= 
                     ((float)triangleVibSpeedStepsInMicros / 
                     (float)vibRateFactor))) {
                     
                     // If the proper amount of microseconds have 
                     // passed and we're counting down, count down one.
                     vibCount --; 
                     
                     // Set the current time for reference the next time.
                     previousVibCountMicros = micros(); 
                  }
              
                  vibAmount = map(vibCount, 0, 200, -100, 100);
                  
                  //Scales vibAmount to very small positive or negative amounts
                  vibAmount2 = (float)vibAmount * vibDepth; 

                  modWheelFactor = map(insideCC1, 0, 127, 0, 2000);
                  vibFactor = 1.0 + (vibAmount2 * 
                     ((float)modWheelFactor/1000));
                  
                  insidePitchInMicrosAfterBendFactor = 
                     calculateInsidePitchInMicrosAfterBendFactorCh2( 
                        insidePitchInMicros, insideBendFactor, 
                        vibFactor, insidePitfall, pitfallTuningOffset);
                  sampleRateMicros = 
                     (float)insidePitchInMicrosAfterBendFactor / 2.0;
              
               }
                
               else { // Squelch IEEE 754 math errors.
                  vibFactor = 1.0;
               }
               
            }
              
            // SINE VIBRATO - creating vibAmount
            if (vibWaveform == 1) {
               if (insideCC1 > 0) {
                  currentVibCountMicros = micros();

                  // Limits how often the chip can calculate the sine function.
                  if  ((currentVibCountMicros - previousVibCountMicros) >= 
                     sineVibSpeedStepsInMicros) { 
                  
                     currentMillis = millis();
                     
                     //range of  1 to -1, sclaed to time, factored to 100 to -100
                     vibAmount = sin(((float)currentMillis * 
                        (float)vibRateFactor)/(float)vibRateDivisor) * 100.0; 
   
                     //Scales vibAmount to very small positive or negative amounts
                     vibAmount2 = (float)vibAmount * vibDepth;
   
                     modWheelFactor = map(insideCC1, 0, 127, 0, 2000);
                     vibFactor = 1.0 + (vibAmount2 * 
                        ((float)modWheelFactor/1000));
                  
                     insidePitchInMicrosAfterBendFactor = 
                        calculateInsidePitchInMicrosAfterBendFactorCh2(
                           insidePitchInMicros, insideBendFactor, 
                           vibFactor, insidePitfall, pitfallTuningOffset);
                     sampleRateMicros = (float)insidePitchInMicrosAfterBendFactor / 2.0;
               
                     previousVibCountMicros = micros();
                  }
               }
                                                        
               else { // Squelch IEEE 754 math errors.
                  vibFactor = 1.0;
               }
            }
              
            // Oscillator guts
                  
            currentMicros = micros(); 
            // If it's time to decrease the pitch, do so, but scale it 
            // non-linearly so that the speed is always the same as if 
            // the shift is passing through 500 Hz.  If the pitch is 
            // higher than 500 Hz, slow the portamento down.  If the 
            // pitch is lower than 500 Hz, speed the portamento up.
            if ((portamentoRunning == HIGH) && (portamentoDirection == LOW) &&
               ((currentMicros - previousPortamentoChange) >= 
               portamentoSpeedInMicros)) {

               portamentoStepInMicros = (portamentoSpeedConstant / 
                  (1.0 / (float)insidePitchInMicros));
               if (portamentoStepInMicros < 1.0) {
                  portamentoStepInMicros = 1;
               }
               insidePitchInMicros = insidePitchInMicros + portamentoStepInMicros;
               previousPortamentoChange = micros();
               
               insidePitchInMicrosAfterBendFactor = 
                  calculateInsidePitchInMicrosAfterBendFactorCh2(
                  insidePitchInMicros, insideBendFactor, vibFactor, 
                  insidePitfall, pitfallTuningOffset);
               sampleRateMicros = (float)insidePitchInMicrosAfterBendFactor / 
                  2.0;
         
               if (insidePitchInMicros >= insideTargetPitchInMicros) {
                  portamentoRunning = LOW;
                  insidePitchInMicros = insideTargetPitchInMicros;
                 
               insidePitchInMicrosAfterBendFactor = 
                  calculateInsidePitchInMicrosAfterBendFactorCh2(
                  insidePitchInMicros, insideBendFactor, vibFactor, 
                  insidePitfall, pitfallTuningOffset);
               sampleRateMicros = 
                  (float)insidePitchInMicrosAfterBendFactor / 2.0;
               }
            }
            
            // If it's time to increase the pitch, do so, but scale 
            // it non-linearly so that the speed is always the same 
            // as if the shift is passing through 500 Hz.  If the 
            // pitch is higher than 500 Hz, slow the portamento down.  
            // If the pitch is lower than 500 Hz, speed the portamento up.
            if ((portamentoRunning == HIGH) && (portamentoDirection == HIGH) 
               && ((currentMicros - previousPortamentoChange) >= 
               portamentoSpeedInMicros)) {
               
               portamentoStepInMicros = (portamentoSpeedConstant / 
                  (1.0 / (float)insidePitchInMicros));
               if (portamentoStepInMicros < 1.0) {
                  portamentoStepInMicros = 1;
               }
               insidePitchInMicros = insidePitchInMicros - 
                  portamentoStepInMicros;
               previousPortamentoChange = micros();

               insidePitchInMicrosAfterBendFactor = 
                  calculateInsidePitchInMicrosAfterBendFactorCh2(
                  insidePitchInMicros, insideBendFactor, vibFactor, 
                  insidePitfall, pitfallTuningOffset);
               
               sampleRateMicros = (float)insidePitchInMicrosAfterBendFactor / 
                  2.0;
                  
               if (insidePitchInMicros <= insideTargetPitchInMicros) {
                  portamentoRunning = LOW;
                  insidePitchInMicros = insideTargetPitchInMicros;
                       
                  insidePitchInMicrosAfterBendFactor = 
                     calculateInsidePitchInMicrosAfterBendFactorCh2(
                        insidePitchInMicros, insideBendFactor, 
                        vibFactor, insidePitfall, pitfallTuningOffset);                         
                  sampleRateMicros = 
                     (float)insidePitchInMicrosAfterBendFactor / 2.0;
              
               }
            }
                         

            currentMicros = micros();
            elapsedTimeInMicros = currentMicros - insidePreviousMicros;
            if (elapsedTimeInMicros >= (unsigned int)sampleRateMicros) {
               if (sampleCounter >= 32) {
                  sampleCounter = 0;
               }

               if (sampleNumber == 1) {
                  current8bitSampleLevel = waveform1[sampleCounter];
               }
   
               switch (sampleNumber) {
                  case 1: current8bitSampleLevel = waveform1[sampleCounter];
                  break;
                  case 2: current8bitSampleLevel = waveform2[sampleCounter];
                  break;
                  case 3: current8bitSampleLevel = waveform3[sampleCounter];
                  break;
                  case 4: current8bitSampleLevel = waveform4[sampleCounter];
                  break;
                  case 5: current8bitSampleLevel = waveform5[sampleCounter];
                  break;
                  case 6: current8bitSampleLevel = waveform6[sampleCounter];
                  break;
                  case 7: current8bitSampleLevel = waveform7[sampleCounter];
                  break;
                  case 8: current8bitSampleLevel = waveform8[sampleCounter];
                  break;
                  case 9: current8bitSampleLevel = waveform9[sampleCounter];
                  break;
                  case 10: current8bitSampleLevel = waveform10[sampleCounter];
                  break;
                  case 11: current8bitSampleLevel = waveform11[sampleCounter];
                  break;
                  case 12: current8bitSampleLevel = waveform12[sampleCounter];
                  break;
               }
               
               analogWrite(A14,current8bitSampleLevel);
                 
               sampleCounter = sampleCounter + sampleIncrementRate;
               insidePreviousMicros = micros();
            }
         }
      break;
      

      // Note On
      case 2: 
      
         if ((insideRun == LOW) && (outsideMidiNoteNumber > 0) && 
            (waveform_Arpeggiator(5,0) == LOW)) {
            
            portamentoRunning = LOW;
            insidePitchInMicros = outsidePitchInMicros;
            insidePreviousMidiNoteNumber = 0;
            insideRun = HIGH;
            insideMidiNoteNumber = outsideMidiNoteNumber;
            
            if (isThisNoteInArray(heldNoteNumberArray,outsideMidiNoteNumber) ==
               LOW) {
            
               j = 0;
               blankSlotFound = LOW;
               while (blankSlotFound == LOW) {
                  if ((heldNoteNumberArray[j] != 0) && (j < 16)) {
                     j++;
                  }
                  else {
                     blankSlotFound = HIGH;
                  }
               }
               heldNoteNumberArray[j] = outsideMidiNoteNumber;
               heldNoteNumberDurationMillis[j] = millis();
            }
         }
              
         if ((waveform_Arpeggiator(5,0) == HIGH) && 
            (outsideMidiNoteNumber > 0)) {
            
            portamentoRunning = LOW;
            insidePitchInMicros = outsidePitchInMicros;
            insidePreviousMidiNoteNumber = 0;
            insideRun = HIGH;
            insideMidiNoteNumber = outsideMidiNoteNumber;
         }
        
         if ((insideRun == HIGH) && (waveform_Arpeggiator(5,0) == 
            LOW) && (outsideMidiNoteNumber > 0)) {
          
            if (isThisNoteInArray(heldNoteNumberArray,
               outsideMidiNoteNumber) == LOW) {
            
               j = 0;
               blankSlotFound = LOW;
               while (blankSlotFound == LOW) {
                  if ((heldNoteNumberArray[j] != 0) && (j < 16)) {
                     j++;
                  }
                  else {
                     blankSlotFound = HIGH;
                  }
               }
               
               heldNoteNumberArray[j] = outsideMidiNoteNumber;
               heldNoteNumberDurationMillis[j] = millis();
            }
          
            j = 0;
            newestNoteDuration = 0;
            for (j = 0; j < 16; j++) {
               if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
                  newestNote = heldNoteNumberArray[j];
               }
            }
            insidePreviousMidiNoteNumber = insideMidiNoteNumber;
            insideMidiNoteNumber = heldNoteNumberArray[j];
            insidePreviousPitchInMicros = insidePitchInMicros;
            insideTargetPitchInMicros = outsidePitchInMicros;
            portamentoRunning = HIGH;
            insideRun = HIGH;
         }
              
         if (portamentoRunning == HIGH) {
            if (insidePreviousPitchInMicros < insideTargetPitchInMicros) {
               //Going down in pitch.  insidePitchInMicros will grow.
               portamentoDirection = LOW; 
            }
            else {
               portamentoDirection = HIGH; //Going up in pitch.  insidePitchInMicros will shrink.
            }
         }
              
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(
               insidePitchInMicros, insideBendFactor, vibFactor, 0, 1);
         sampleRateMicros = (float)insidePitchInMicrosAfterBendFactor / 2.0;
         waveform_Arpeggiator(9,0);
      break;
      

      // Note Off
      case 3:
         if (outsideMidiNoteNumber > 0) {
          
            // If the arpeggiator is turned off OR there are two notes 
            // held down by the arpeggiator.
            j = 0;
            highestNoteNumber = 0;
            for (j = 0; j < 16; j++) {
               if (heldNoteNumberArray[j] == outsideMidiNoteNumber) {
                  heldNoteNumberDurationMillis[j] = 0;
                  heldNoteNumberArray[j] = 0;
               }
               if (heldNoteNumberArray[j] > 0) {
               highestNoteNumber = heldNoteNumberArray[j];
               }  
            }
            if (highestNoteNumber == 0) {
               insideRun = LOW;
               digitalWriteFast(audioTwo, LOW);
               insidePreviousMidiNoteNumber = 0;
            }
            else {
               j = 0;
               newestNoteDuration = 0;
               for (j = 0; j < 16; j++) {
                  if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
                     newestNote = heldNoteNumberArray[j];
                  }
               }
               if ((waveform_Arpeggiator(5,0) == LOW) || 
                  ((waveform_Arpeggiator(5,0) == HIGH) && 
                  (waveform_Arpeggiator(7,0) == HIGH) && 
                  (waveform_Arpeggiator(8,0) == LOW))) {
      
                  OnNoteOn(6,newestNote,127);
               }
            }
          
            insidePitchInMicrosAfterBendFactor = 
               calculateInsidePitchInMicrosAfterBendFactorCh2(
               insidePitchInMicros, insideBendFactor, vibFactor, 0, 1);

            sampleRateMicros = (float)insidePitchInMicrosAfterBendFactor / 2.0;
            waveform_Arpeggiator(9,
               testArrayContentsForTwoNotesOrMore(heldNoteNumberArray));
         }
                   
      break;

      // Pitch Bend
      case 4: 
         insideBendFactor = outsideBendFactor;
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(insidePitchInMicros, 
            insideBendFactor, vibFactor, 0, 1);
         sampleRateMicros = (float)insidePitchInMicrosAfterBendFactor / 2.0;
      break;
      
      // CC 1 Change
      case 5: 
         insideCC1 = outsideCC;
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(insidePitchInMicros, 
            insideBendFactor, vibFactor, 0, 1);
         sampleRateMicros = (float)insidePitchInMicrosAfterBendFactor / 2.0;
                
      break;
      
      // CC 2 Change (Reserved for Bend Range)
      case 6:
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(insidePitchInMicros, 
            insideBendFactor, vibFactor, 0, 1);
         sampleRateMicros = (float)insidePitchInMicrosAfterBendFactor / 2.0;
      break;
      
      // CC 3 Change (vibrato type)
      case 7:
         insideCC3 = outsideCC;
         if (insideCC3 > 0) {
            vibWaveform = 1;
         }
         else {
            vibWaveform = 0;
         }
        
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(
            insidePitchInMicros, insideBendFactor, vibFactor, 0, 1);
         sampleRateMicros = (float)insidePitchInMicrosAfterBendFactor / 2.0;
      break;
      
      // CC 4 Change (Reserved for Arpeggiator On)
      case 8:
      break;
      
      // CC 6 Change (Sample Number)
      case 9:
         sampleNumber = map(outsideCC,0,127,1,12);
      break;
      
      // Returns newest note to Arpeggiator when it turns off.
      case 10:
         j = 0;
         newestNoteDuration = 0;
         for (j = 0; j < 16; j++) {
            if ((heldNoteNumberDurationMillis[j] > newestNoteDuration)) {
               newestNote = heldNoteNumberArray[j];
               newestNoteDuration = heldNoteNumberDurationMillis[j];
            }      
         }
        
         insidePitchInMicrosAfterBendFactor = 
            calculateInsidePitchInMicrosAfterBendFactorCh2(insidePitchInMicros, 
            insideBendFactor, vibFactor, 0, 1);
         sampleRateMicros = (float)insidePitchInMicrosAfterBendFactor / 2.0;
              
         return newestNote;
      break;
      
      case 11: //Returns insideDecay on other three oscillators
         return LOW;
      break;

      case 12:
         portamentoSpeedInMicros = map(outsideCC,0,127,20,2000);
      break;
   }
}
