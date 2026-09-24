#include <Servo.h>

const int irLedMid = 6;
const int irSensorMid = 7;
const int ledMid = A1;

const int irLedLeft = 10;
const int irSensorLeft = 11;
const int ledLeft = A2;

const int irLedRight = 2;
const int irSensorRight = 3;
const int ledRight = A0;

const int leftServoStop = 1500;
const int rightServoStop = 1490;



Servo servoLeft;
Servo servoRight;

void setup() {
    servoLeft.attach(13);
    servoRight.attach(12);
    pinMode(irSensorLeft, INPUT);            // IR receiver pin is an input
    pinMode(irLedLeft, OUTPUT);                // IR LED pin is an output
    pinMode(ledLeft, OUTPUT);               // Red LED pin is an output

    // Middle
    pinMode(irSensorMid, INPUT);          // IR receiver pin is an input
    pinMode(irLedMid, OUTPUT);              // IR LED pin is an output
    pinMode(ledMid, OUTPUT);             // Red LED pin is an output

    // Right
    pinMode(irSensorRight, INPUT);           // IR receiver pin is an input
    pinMode(irLedRight, OUTPUT);               // IR LED pin is an output
    pinMode(ledRight, OUTPUT);            // Red LED pin is an output
    Serial.begin(9600);   

    stop();
    centreAtStart();
}

void loop() {

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




  if ((irDetect(irLedRight, irReceiverRight, 38000) == 0) && (irDetect(irLedLeft, irReceiverLeft, 38000) == 0)) {
      forwardAndCount();
  }
  

  if ((irDetect(irLedLeft, irReceiverLeft, 38000) == 1) && (irDetect(irLedRight, irReceiverRight, 38000) == 0)) {

      turnLeft();
  }
  
  if ((irDetect(irLedRight, irReceiverRight, 38000) == 1) && ((irDetect(irLedLeft, irReceiverLeft, 38000) == 0))) {
      turnRight();
  }

   if ((irDetect(irLedRight, irReceiverRight, 38000) == 1) && ((irDetect(irLedLeft, irReceiverLeft, 38000) == 1))) {
      stop();
  }
  /*
  if ((irDetect(irLedRight, irReceiverRight, 38000) == 0) && (irDetect(irLedLeft, irReceiverLeft, 38000) == 0)) {
     servoLeft.writeMicroseconds(1610);  // 1.3ms = full speed clockwise
  servoRight.writeMicroseconds(1610);
  Serial.println("Right for 3 seconds - no walls on either side");
  delay(2000);
       servoRight.writeMicroseconds(1360);  // 1.3ms = full speed clockwise
  servoLeft.writeMicroseconds(1640);
    Serial.println("Forward so it doesn't stay in an endless loop of circling clockwise");
    delay(2000);
    
  }
*/
digitalWrite(redLedLeft, LOW);
  digitalWrite(redLedMiddle, LOW);
  digitalWrite(redLedRight, LOW);


}

int irDistance(int irLedPin, int irSensorPin, long intercept, long increment) {
   int distance = 0;
   for(long frequency = intercept; frequency <= (intercept + (increment * 5)); frequency += increment)
   {
      distance += irDetect(irLedPin, irReceiverPin, f);
   }
   Serial.println(distance);
   return distance;

}

int irDetect(int irLedPin, int irSensorPin, long frequency) {
    tone(irLedPin, frequency);                 // Turn on the IR LED square wave
    delay(1);                                  // Wait 1 ms
    int ir = digitalRead(irSensorPin);       // IR receiver -> ir variable
    noTone(irLedPin);                          // Turn off the IR LED
    delay(1);                                  // Down time before recheck
    return ir;                                 // Return 0 detect, 1 no detect
}

void goStraightOne() {

}

void turnLeft() {
    servoLeft.writeMicroseconds(leftServoStop - 44);
    servoRight.writeMicroseconds(rightServoStop - 44);
    delay(1000);
    stop();
}


void turnRight() {
    servoLeft.writeMicroseconds(leftServoStop + 43);
    servoRight.writeMicroseconds(rightServoStop + 43);
    delay(1000);
    stop();
}

void centre() {

}

void centreAtStart() {
    frontBlocked = irDetect(irLedMid, irSensorMid, 38000);
    if (frontBlocked == 0) {
        while (irDetect(irLedMid, irSensorMid, 38000) == 0) {
            leftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
            rightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
            //while (leftDistance != rightDistance) {
                leftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
                rightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
                Serial.print("left: ");
                Serial.println(leftDistance);
                Serial.print("right: ");
                Serial.println(rightDistance);
                if (leftDistance < rightDistance) {
                    servoLeft.writeMicroseconds(leftServoStop + 43);
                    servoRight.writeMicroseconds(rightServoStop + 43);
                    delay(200);
                    stop();
                } else if (leftDistance > rightDistance) {
                    servoLeft.writeMicroseconds(leftServoStop - 44);
                    servoRight.writeMicroseconds(rightServoStop - 44);
                    delay(200);
                    stop();
                } else {
                    Serial.println("stuck :(");
                }

            //}
        }
    }

    }
}

void stop() {
    servoLeft.writeMicroseconds(1500);
    servoRight.writeMicroseconds(1490);
}

int forwardAndCount() {
    int count = 0;
    while (irDetect(irLedMid, irSensorMid, 45000) == 1) {
        servoLeft.writeMicroseconds(leftServoStop + 40);
        servoRight.writeMicroseconds(rightServoStop - 40);
        count++;
        delay(100);
    }
    stop();
    Serial.println("Count: " + String(count));
    return count;
}