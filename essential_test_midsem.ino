// This script was copied from "part_1 copy.ino" at 5:40pm Tuesday, September 29.
// I want to edit the hell out of this to try and figure out a proper way to get the relative positioning and velocity 
// of the robot because it would better inform code when figuring out what case to run - going straight, centring at
// the start, minor centring corrections, turning, and detecting soft/dead ends

#include <Servo.h>

const int irLedMiddle=6, irReceiverMiddle=7;   // Select these to match the IR LED/receiver pair that you are using
const int redLedMiddle = A1;                    // Select this to match the red LED next to the IR receiver you are using

const int irLedLeft=10, irReceiverLeft=11;
const int redLedLeft = A2;  

const int irLedRight=2, irReceiverRight=3;
const int redLedRight = A0;       

Servo servoLeft;
Servo servoRight;



void setup()                                 // Built-in initialization block
{
// Left
servoLeft.attach(13);
servoRight.attach(12);
   pinMode(irReceiverLeft, INPUT);            // IR receiver pin is an input
   pinMode(irLedLeft, OUTPUT);                // IR LED pin is an output
   pinMode(redLedLeft, OUTPUT);               // Red LED pin is an output

   // Middle
   pinMode(irReceiverMiddle, INPUT);          // IR receiver pin is an input
   pinMode(irLedMiddle, OUTPUT);              // IR LED pin is an output
   pinMode(redLedMiddle, OUTPUT);             // Red LED pin is an output

   // Right
   pinMode(irReceiverRight, INPUT);           // IR receiver pin is an input
   pinMode(irLedRight, OUTPUT);               // IR LED pin is an output
   pinMode(redLedRight, OUTPUT);            // Red LED pin is an output
   Serial.begin(9600);                       // Set data rate to 9600 bps
}  
 
void loop()
{
  //need three sections: 1. identifying position and making a decision
  // 2. 0.2s delay to ensure LED gets lit ig? can remove after debugging process
  //conduct the decision
  int irValLeft = irDetect(irLedLeft, irReceiverLeft, 38000);               // Check for object
  //Serial.println(irVal);                     // Display 1/0 no detect/detect
  if (irValLeft == 0)          // Optional - display detection by setting red LED high
  {
    digitalWrite(redLedLeft, HIGH); 
    Serial.print("left: ");
   Serial.println(irDistance(irLedLeft, irReceiverLeft, 38500, 1000)); 
  }

   int irValMiddle = irDetect(irLedMiddle, irReceiverMiddle, 38000);               // Check for object
  //Serial.println(irVal);                     // Display 1/0 no detect/detect
  if (irValMiddle == 0)          // Optional - display detection by setting red LED high
  {
    digitalWrite(redLedMiddle, HIGH); 
    Serial.print("middle: ");
   Serial.println(irDistance(irLedMiddle, irReceiverMiddle, 38000, 1000)); 
  }

  int irValRight = irDetect(irLedRight, irReceiverRight, 38000);               // Check for object
  //Serial.println(irVal);                     // Display 1/0 no detect/detect
  if (irValRight == 0)          // Optional - display detection by setting red LED high
  {
    digitalWrite(redLedRight, HIGH); 
    Serial.print("right: ");
   Serial.println(irDistance(irLedRight, irReceiverRight, 38000, 1000)); 
  }

  
                            // 0.5 second delay - just long enough to see the LED blink
  digitalWrite(redLedLeft, LOW);
  digitalWrite(redLedMiddle, LOW);
  digitalWrite(redLedRight, LOW);


digitalWrite(redLedLeft, LOW);
  digitalWrite(redLedMiddle, LOW);
  digitalWrite(redLedRight, LOW);

  }

//should ideally have mappings from each frequency to each distance for each LED. this can be done with
//the take home kit, then when going to the actual robot, the values can be swapped based on testing.
//this mapping shuold be done with an increment of like 1000, going from 38k to 45k or whatever value was appropriate.

// IR Object Detection Function
int irDistance(int irLedPin, int irReceiverPin, long intercept, long increment)
{
   int distance = 0;
   for(long f = intercept; f <= (intercept + (increment * 5)); f += increment)
   {
      distance += irDetect(irLedPin, irReceiverPin, f);
   }
   return distance;
}

int irZone(int irLedPin, int irReceiverPin, )


int irDetect(int irLedPin, int irReceiverPin, long frequency)
{
  tone(irLedPin, frequency);                 // Turn on the IR LED square wave
  delay(1);                                  // Wait 1 ms
  int ir = digitalRead(irReceiverPin);       // IR receiver -> ir variable
  noTone(irLedPin);                          // Turn off the IR LED
  delay(1);                                  // Down time before recheck
  return ir;                                 // Return 0 detect, 1 no detect
}
