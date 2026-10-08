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

const int timeToTurnLeft = 1500;
const int timeToTurnRight = 1430;


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
    allLightsOff();


  int leftSideClear = irDetect(irLedLeft, irSensorLeft, 38000);
  if (leftSideClear == 0) // 0 means wall detected
  {
    digitalWrite(ledLeft, HIGH); 
    Serial.print("left: ");
    int leftDistance = irDistance(irLedLeft, irSensorLeft, 38500, 1000);
    Serial.println(leftDistance);
  }

    int middleClear = irDetect(irLedMid, irSensorMid, 38000);
  if (middleClear == 0)// 0 means wall detected
  {
    digitalWrite(ledMid, HIGH); 
    Serial.print("Mid: ");
    int midDistance = irDistance(irLedMid, irSensorMid, 38000, 1000);
    Serial.println(midDistance);
  }

  int rightSideClear = irDetect(irLedRight, irSensorRight, 38000);
  if (rightSideClear == 0) // 0 means wall detected
  {
    digitalWrite(ledRight, HIGH); 
    Serial.print("right: ");
    int rightDistance = (irDistance(irLedRight, irSensorRight, 38000, 1000));
    Serial.println(rightDistance); 
  }

 

  digitalWrite(ledLeft, LOW);
  digitalWrite(ledMid, LOW);
  digitalWrite(ledRight, LOW);


  
  delay(500);



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

void goForwardFive() {
    servoLeft.writeMicroseconds(leftServoStop + 40);
    servoRight.writeMicroseconds(rightServoStop - 41);
    delay(1500);
    stop();
}

void goForwardThree() {
    servoLeft.writeMicroseconds(leftServoStop + 40);
    servoRight.writeMicroseconds(rightServoStop - 41);
    delay(900);
    stop();
}

void goBackFive() {
    servoLeft.writeMicroseconds(leftServoStop - 41);
    servoRight.writeMicroseconds(rightServoStop + 40);
    delay(1500);
    stop();
}

void goBackThree() {
    servoLeft.writeMicroseconds(leftServoStop - 41);
    servoRight.writeMicroseconds(rightServoStop + 40);
    delay(900);
    stop();

}

void leftTurn90() {
    servoLeft.writeMicroseconds(leftServoStop - 20);
    servoRight.writeMicroseconds(rightServoStop - 44);
    delay(timeToTurnLeft);
    stop();
}

void rightTurn90() {
    servoLeft.writeMicroseconds(leftServoStop + 43);
    servoRight.writeMicroseconds(rightServoStop + 21);
    delay(timeToTurnRight);
    stop();
}

void leftTurn30() {
    servoLeft.writeMicroseconds(leftServoStop - 20);
    servoRight.writeMicroseconds(rightServoStop - 44);
    delay(timeToTurnLeft / 3);
    stop();
}

void rightTurn30() {
    servoLeft.writeMicroseconds(leftServoStop + 43);
    servoRight.writeMicroseconds(rightServoStop + 21);
    delay(timeToTurnRight / 3);
    stop();
}

void leftTurn15() {
    servoLeft.writeMicroseconds(leftServoStop - 20);
    servoRight.writeMicroseconds(rightServoStop - 44);
    delay(timeToTurnLeft / 6);
    stop();
}

void rightTurn15() {
    servoLeft.writeMicroseconds(leftServoStop + 43);
    servoRight.writeMicroseconds(rightServoStop + 21);
    delay(timeToTurnRight / 6);
    stop();
}

void centreToWall(String wall) {
    //need to write
}

void centreAtStart() {
    //int frontBlocked = irDetect(irLedMid, irSensorMid, 38000);
    //if (frontBlocked == 0) {
        //while (irDetect(irLedMid, irSensorMid, 38000) == 0) {
            //servoLeft.writeMicroseconds(leftServoStop - 43);
            //servoRight.writeMicroseconds(rightServoStop + 43);
            //delay(50);
            //stop();
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
        

    //}

    
}

void stop() {
    servoLeft.writeMicroseconds(leftServoStop);
    servoRight.writeMicroseconds(rightServoStop);
}

/*int forwardAndCount() {
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
*/

void allLightsOff() {
    digitalWrite(ledLeft, LOW);
    digitalWrite(ledMid, LOW);
    digitalWrite(ledRight, LOW);
}