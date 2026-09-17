const int irLedPin=6, irReceiverPin=7;   // Select these to match the IR LED/receiver pair that you are using
const int redLedPin = A1;                    // Select this to match the red LED next to the IR receiver you are using

void setup()                                 // Built-in initialization block
{
   pinMode(irReceiverPin, INPUT);            // IR receiver pin is an input
   pinMode(irLedPin, OUTPUT);                // IR LED pin is an ouput
   pinMode(redLedPin, OUTPUT);               // Red LED pin is an output
   Serial.begin(9600);                       // Set data rate to 9600 bps
}  
 
void loop()                                  // Main loop auto-repeats
{
  int irVal = irDetect(38000);               // Check for object
  Serial.println(irVal);                     // Display 1/0 no detect/detect
  if (irVal == 0)          // Optional - display detection by setting red LED high
  {
    digitalWrite(redLedPin, HIGH); 
   Serial.printLn(irDistance(irLedPin, irReceiverPin)); 

  }
  delay(500);                                // 0.5 second delay - just long enough to see the LED blink
  digitalWrite(redLedPin, LOW);
}

// IR Object Detection Function
int irDistance(int irLedPin, int irReceiverPin)
{
   int distance = 0;
   for(long f = 38000; f <= 45000; f += 1000)
   {
      distance += irDetect(irLedPin, irReceiverPin, f);
   }
   return distance;
}


int irDetect(long frequency)
{
  tone(irLedPin, frequency);                 // Turn on the IR LED square wave
  delay(1);                                  // Wait 1 ms
  int ir = digitalRead(irReceiverPin);       // IR receiver -> ir variable
  noTone(irLedPin);                          // Turn off the IR LED
  delay(1);                                  // Down time before recheck
  return ir;                                 // Return 0 detect, 1 no detect
}
