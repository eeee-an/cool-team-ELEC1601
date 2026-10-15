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

const int timeToTurnLeft = 1250;
const int timeToTurnRight = 1050;

//const int wideTurn = 40;
//const int narrowTurn = 20;

//need: starting code, go forward until either front is detected or L and R change
// left turn (perfect)
// right turn (perfect)
// dead end (persistent variable to make sure only one dead end has been reached)
// 180 degree turn (perfect)

void setup() {


}

void loop() {
    bool leftBlocked = false;
    bool frontBlocked = false;
    bool rightBlocked = false;

    int leftDistance = 999;
    int midDistance = 999;
    int rightDistance = 999;

    // Debug reads
    int leftSideClear = irDetect(irLedLeft, irSensorLeft, 38000);
    if (leftSideClear == 0) { // 0 means wall detected
        leftBlocked = true;
        digitalWrite(ledLeft, HIGH); 
        Serial.print("left: ");
        leftDistance = irDistance(irLedLeft, irSensorLeft, 38500, 1000);
        Serial.println(leftDistance);
    }

    int middleClear = irDetect(irLedMid, irSensorMid, 38000);
    if (middleClear == 0) { // 0 means wall detected
        frontBlocked = true;
        digitalWrite(ledMid, HIGH); 
        Serial.print("Mid: ");
        midDistance = irDistance(irLedMid, irSensorMid, 38000, 1000);
        Serial.println(midDistance);
    }

    int rightSideClear = irDetect(irLedRight, irSensorRight, 38000);
    if (rightSideClear == 0) { // 0 means wall detected
        rightBlocked = true;
        digitalWrite(ledRight, HIGH); 
        Serial.print("right: ");
        rightDistance = (irDistance(irLedRight, irSensorRight, 38000, 1000));
        Serial.println(rightDistance); 
    }
    delay(750);
    allLightsOff();

    if (leftBlocked && rightBlocked) {
        if (abs(leftDistance - rightDistance) < 2) {
            if (!frontBlocked) {
                // 001: Middle of long corridor
                Serial.println("001: Middle of long corridor");
                showLights(0, 0, 1);
                delay(3000);
                forwardLoop();
                stopWithLights();
            } else {
                // 100: Dead End
                Serial.println("100: Dead End");
                showLights(1, 0, 0);
                delay(3000);
                //insert if statement here to see if it's the first dead end
                uturn();
                delay(500);
                goForwardFive();
                stopWithLights();
            }
        }
    } else if ()//turning cases

}

void goForwardFive() {
    servoLeft.writeMicroseconds(leftServoStop + 40);
    servoRight.writeMicroseconds(rightServoStop - 41);
    forwardChevron(); // Fire lights during movement
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

void forwardLoop() {
    int frontClear = 1;
    int leftDistance = irDistance(irLedLeft, irSensorLeft, 38500, 1000);
    int rightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
    while (frontClear == 1) {
        goForwardFive();
    
        int newLeftDistance = irDistance(irLedLeft, irSensorLeft, 38500, 1000);
        int newRightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);

        if (newLeftDistance == leftDistance && newRightDistance == rightDistance) {
            //good
        } else if (newLeftDistance < leftDistance || newRightDistance > rightDistance) {
            // veered left
            rightTurn5();
        } else if (newLeftDistance > leftDistance || newRightDistance > rightDistance) {
            // veered right
            leftTurn5();
        }
        
        leftDistance = irDistance(irLedLeft, irSensorLeft, 38500, 1000);
        rightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);

        frontClear = irDetect(irLedMid, irSensor, 38000);
        
    }
    stop();
}


void leftTurn90() {
    servoLeft.writeMicroseconds(leftServoStop - 20);
    servoRight.writeMicroseconds(rightServoStop - 44);
    leftChevron(); // Fire directional lights
    delay(timeToTurnLeft);
    stop();
}

void rightTurn90() {
    servoLeft.writeMicroseconds(leftServoStop + 43);
    servoRight.writeMicroseconds(rightServoStop + 21);
    rightChevron(); // Fire directional lights
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

void reverseLeftTurn30() {
    servoLeft.writeMicroseconds(leftServoStop + 20);
    servoRight.writeMicroseconds(rightServoStop + 44);
    delay(timeToTurnLeft / 3);
    stop();
}

void reverseRightTurn30() {
    servoLeft.writeMicroseconds(leftServoStop - 43);
    servoRight.writeMicroseconds(rightServoStop - 21);
    delay(timeToTurnRight / 3);
    stop();
}

void leftTurn5() {
    servoLeft.writeMicroseconds(leftServoStop - 20);
    servoRight.writeMicroseconds(rightServoStop - 44);
    delay(timeToTurnLeft / 18);
    stop();
}

void rightTurn5() {
    servoLeft.writeMicroseconds(leftServoStop + 43);
    servoRight.writeMicroseconds(rightServoStop + 21);
    delay(timeToTurnRight / 18);
    stop();
}

void uturn() {
    servoLeft.writeMicroseconds(leftServoStop - 40);
    servoRight.writeMicroseconds(rightServoStop - 40);
    delay(696); 
    servoLeft.writeMicroseconds(leftServoStop - 41);
    servoRight.writeMicroseconds(rightServoStop + 40);
    delay(200);
    servoLeft.writeMicroseconds(leftServoStop - 40);
    servoRight.writeMicroseconds(rightServoStop - 40);
    delay(696); 
    servoLeft.writeMicroseconds(leftServoStop - 41);
    servoRight.writeMicroseconds(rightServoStop + 40);
    delay(200);
    servoLeft.writeMicroseconds(leftServoStop - 40);
    servoRight.writeMicroseconds(rightServoStop - 40);
    delay(696); 
    servoLeft.writeMicroseconds(leftServoStop - 41);
    servoRight.writeMicroseconds(rightServoStop + 40);
    delay(200);
    stop();
}

void stop() {
    servoLeft.writeMicroseconds(leftServoStop);
    servoRight.writeMicroseconds(rightServoStop);
}

int irDistance(int irLedPin, int irSensorPin, long intercept, long increment) {
   int distance = 0;
   for(long frequency = intercept; frequency <= (intercept + (increment * 5)); frequency += increment)
   {
      distance += irDetect(irLedPin, irSensorPin, frequency);
   }
   return distance;
}

int irDetect(int irLedPin, int irSensorPin, long frequency) {
    tone(irLedPin, frequency);                 
    delay(1);                                  
    int ir = digitalRead(irSensorPin);       
    noTone(irLedPin);                          
    delay(1);                                  
    return ir;                                 
}

void showLights(int l, int m, int r) {
    digitalWrite(ledLeft, l);
    digitalWrite(ledMid, m);
    digitalWrite(ledRight, r);
}
