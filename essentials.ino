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
    pinMode(irSensorLeft, INPUT);            // IR Sensor pin is an input
    pinMode(irLedLeft, OUTPUT);                // IR LED pin is an output
    pinMode(ledLeft, OUTPUT);               // Red LED pin is an output

    // Mid
    pinMode(irSensorMid, INPUT);          // IR Sensor pin is an input
    pinMode(irLedMid, OUTPUT);              // IR LED pin is an output
    pinMode(ledMid, OUTPUT);             // Red LED pin is an output

    // Right
    pinMode(irSensorRight, INPUT);           // IR Sensor pin is an input
    pinMode(irLedRight, OUTPUT);               // IR LED pin is an output
    pinMode(ledRight, OUTPUT);            // Red LED pin is an output
    Serial.begin(9600);   

    stop();
    centreAtStart();
}

void loop() {


  int irValLeft = irDetect(irLedLeft, irSensorLeft, 38000);               // Check for object
  //Serial.println(irVal);                     // Display 1/0 no detect/detect
  if (irValLeft == 0)          // Optional - display detection by setting red LED high
  {
    digitalWrite(ledLeft, HIGH); 
    Serial.print("left: ");
   Serial.println(irDistance(irLedLeft, irSensorLeft, 38500, 1000)); 
  }

   int irValMid = irDetect(irLedMid, irSensorMid, 38000);               // Check for object
  //Serial.println(irVal);                     // Display 1/0 no detect/detect
  if (irValMid == 0)          // Optional - display detection by setting red LED high
  {
    digitalWrite(ledMid, HIGH); 
    Serial.print("Mid: ");
   Serial.println(irDistance(irLedMid, irSensorMid, 38000, 1000)); 
  }

  int irValRight = irDetect(irLedRight, irSensorRight, 38000);               // Check for object
  //Serial.println(irVal);                     // Display 1/0 no detect/detect
  if (irValRight == 0)          // Optional - display detection by setting red LED high
  {
    digitalWrite(ledRight, HIGH); 
    Serial.print("right: ");
   Serial.println(irDistance(irLedRight, irSensorRight, 38000, 1000)); 
  }

  
                            // 0.5 second delay - just long enough to see the LED blink
  digitalWrite(ledLeft, LOW);
  digitalWrite(ledMid, LOW);
  digitalWrite(ledRight, LOW);




  if ((irDetect(irLedRight, irSensorRight, 38000) == 0) && (irDetect(irLedLeft, irSensorLeft, 38000) == 0)) {
      forwardAndCount();
  }
  

  if ((irDetect(irLedLeft, irSensorLeft, 38000) == 1) && (irDetect(irLedRight, irSensorRight, 38000) == 0)) {

      turnLeft();
      forwardAndCount();
  }
  
  if ((irDetect(irLedRight, irSensorRight, 38000) == 1) && ((irDetect(irLedLeft, irSensorLeft, 38000) == 0))) {
      turnRight();
      forwardAndCount();
  }

   if ((irDetect(irLedRight, irSensorRight, 38000) == 1) && ((irDetect(irLedLeft, irSensorLeft, 38000) == 1))) {
      stop();
  }
  /*
  if ((irDetect(irLedRight, irSensorRight, 38000) == 0) && (irDetect(irLedLeft, irSensorLeft, 38000) == 0)) {
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
digitalWrite(ledLeft, LOW);
  digitalWrite(ledMid, LOW);
  digitalWrite(ledRight, LOW);


}

int irDistance(int irLedPin, int irSensorPin, long intercept, long increment) {
   int distance = 0;
   for(long frequency = intercept; frequency <= (intercept + (increment * 5)); frequency += increment)
   {
      distance += irDetect(irLedPin, irSensorPin, frequency);
   }
   Serial.println(distance);
   return distance;

}

int irDetect(int irLedPin, int irSensorPin, long frequency) {
    tone(irLedPin, frequency);                 // Turn on the IR LED square wave
    delay(1);                                  // Wait 1 ms
    int ir = digitalRead(irSensorPin);       // IR Sensor -> ir variable
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
    int frontBlocked = irDetect(irLedMid, irSensorMid, 37000);
    if (frontBlocked == 0) {
        //while (irDetect(irLedMid, irSensorMid, 37000) == 0) {
            //servoLeft.writeMicroseconds(leftServoStop - 43);
            //servoRight.writeMicroseconds(rightServoStop + 43);
            delay(50);
            stop();
            int leftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
            int rightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
            while (leftDistance != rightDistance) {
                leftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
                rightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
                Serial.print("left: ");
                Serial.println(leftDistance);
                Serial.print("right: ");
                Serial.println(rightDistance);
                if (leftDistance < rightDistance) {
                    servoLeft.writeMicroseconds(leftServoStop + 43);
                    servoRight.writeMicroseconds(rightServoStop + 43);
                    delay(50);
                    stop();
                } else if (leftDistance > rightDistance) {
                    servoLeft.writeMicroseconds(leftServoStop - 44);
                    servoRight.writeMicroseconds(rightServoStop - 44);
                    delay(50);
                    stop();
                } else {
                    Serial.println("stuck :(");
                }

            }
        //}
        

    }

    
}

void stop() {
    servoLeft.writeMicroseconds(1500);
    servoRight.writeMicroseconds(1490);
}

int forwardAndCount() {
    int count = 0;
    while (irDetect(irLedMid, irSensorMid, 44000) == 1) {
        servoLeft.writeMicroseconds(leftServoStop + 40);
        servoRight.writeMicroseconds(rightServoStop - 41);
        count++;
        delay(100);
    }
    stop();
    Serial.println("Count: " + String(count));
    return count;
}