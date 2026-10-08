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
    
    pinMode(irSensorLeft, INPUT);
    pinMode(irLedLeft, OUTPUT);
    pinMode(ledLeft, OUTPUT);

    pinMode(irSensorMid, INPUT);
    pinMode(irLedMid, OUTPUT);
    pinMode(ledMid, OUTPUT);

    pinMode(irSensorRight, INPUT);
    pinMode(irLedRight, OUTPUT);
    pinMode(ledRight, OUTPUT);
    
    Serial.begin(9600);   

    stop();
}

void loop() {
    stop();
    delay(1000);
    allLightsOff();

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
    
    delay(3000);
    allLightsOff();

    // --- Navigation Logic ---
    if (leftBlocked && rightBlocked) {
        if (leftDistance == rightDistance) {
            if (!frontBlocked) {
                // 001: Middle of long corridor
                Serial.println("001: Middle of long corridor");
                displaySituationCode(0, 0, 1);
                goForwardFive();
                stopWithLights();
            } else {
                // 100: Dead End
                Serial.println("100: Dead End");
                displaySituationCode(1, 0, 0);
                uturn();
                delay(500);
                goForwardFive();
                stopWithLights();
            }
        } 
        else if (leftBlocked && frontBlocked && !rightBlocked) {
            Serial.println("LEFT and FRONT blocked");
            rightTurn30();

            int newRightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
            
            if (!rightBlocked) {
                // 011: Ideal right turn
                Serial.println("011: Ideal right turn");
                displaySituationCode(0, 1, 1);
                reverseRightTurn30(); // reset angle
                delay(500);
                rightTurn90();
                delay(500);
                goForwardFive();
                stopWithLights();
            } else if (leftDistance < midDistance) {
                // 111: Bad start, angled left
                Serial.println("111: Bad start, angled left");
                displaySituationCode(1, 1, 1);
                centreToRightWall();
                stopWithLights();
            } else {
                unknownSituation();
            }
        }
    } 
    else if (rightBlocked && frontBlocked && !leftBlocked) {
        Serial.println("RIGHT and FRONT blocked");
        leftTurn30();

        int newLeftDistance = irDistance(irLedLeft, irSensorLeft, 38500, 1000);
        
        if (!leftBlocked) {
            // 010: Ideal left turn
            Serial.println("010: Ideal left turn");
            displaySituationCode(0, 1, 0);
            reverseLeftTurn30(); // reset angle
            delay(500);
            leftTurn90();
            delay(500);
            goForwardFive();
            stopWithLights();
        } else if (rightDistance < midDistance) {
            // 001: Bad start, angled right
            Serial.println("001: Bad start, angled right");
            displaySituationCode(0, 0, 1);
            centreToLeftWall();
            stopWithLights();
        } else {
            unknownSituation();
        }
    } 
    else if (!frontBlocked && rightBlocked && leftBlocked) {
        Serial.println("Corridor parallel adjustments");
        int adjustments = 0;
        
        // Loop while difference is > 1 and adjustments < 6
        while (abs(rightDistance - leftDistance) > 1 && adjustments < 6) {
            if (leftDistance < rightDistance) {
                // 101: Bad start, left parallel
                Serial.println("101: Bad start, left parallel");
                displaySituationCode(1, 0, 1);
                rightTurn15();
                delay(500);
                goForwardThree();
                delay(500);
                leftTurn15();
                delay(500);
                goBackThree();
                adjustments++;
            } else {
                // 110: Bad start, right parallel
                Serial.println("110: Bad start, right parallel");
                displaySituationCode(1, 1, 0);
                leftTurn15();
                delay(500);
                goForwardThree();
                delay(500);
                rightTurn15();
                delay(500);
                goBackThree();
                adjustments++;
            }
            // Re-check distances for the while loop
            leftDistance = irDistance(irLedLeft, irSensorLeft, 38500, 1000);
            rightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
            delay(500);
        }
        stopWithLights();
    } 
    else {
        unknownSituation();
    }

    delay(10000); // Wait 10 seconds before next loop
}

// -------------------------------------------------------------
// SENSOR FUNCTIONS
// -------------------------------------------------------------

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

// -------------------------------------------------------------
// LED NOTIFICATION FUNCTIONS
// -------------------------------------------------------------

// Displays the 3-digit code on the LEDs and pauses for 5 seconds
void displaySituationCode(int l, int m, int r) {
    digitalWrite(ledLeft, l ? HIGH : LOW);
    digitalWrite(ledMid, m ? HIGH : LOW);
    digitalWrite(ledRight, r ? HIGH : LOW);
    delay(5000); // Wait 5 seconds so you can read the code
}

void stopWithLights() {
    stop();
    // STOP: 3x left right flashing quick
    for(int i = 0; i < 3; i++) {
        digitalWrite(ledLeft, HIGH);
        digitalWrite(ledRight, HIGH);
        delay(100);
        digitalWrite(ledLeft, LOW);
        digitalWrite(ledRight, LOW);
        delay(100);
    }
}

void unknownSituation() {
    Serial.println("000: Unknown situation");
    displaySituationCode(0, 0, 0);
    // Flashes 3x by calling stopWithLights 3 times
    stopWithLights();
    stopWithLights();
    stopWithLights();
}

void forwardChevron() {
    // left right on, then quickly turn mid on, quickly turn left and right off
    digitalWrite(ledLeft, HIGH);
    digitalWrite(ledRight, HIGH);
    delay(100);
    digitalWrite(ledMid, HIGH);
    delay(100);
    digitalWrite(ledLeft, LOW);
    digitalWrite(ledRight, LOW);
    delay(100);
    digitalWrite(ledMid, LOW);
}

void leftChevron() {
    digitalWrite(ledLeft, HIGH);
    delay(100);
    digitalWrite(ledMid, HIGH);
    delay(100);
    digitalWrite(ledLeft, LOW);
    delay(100);
    digitalWrite(ledMid, LOW);
}

void rightChevron() {
    digitalWrite(ledRight, HIGH);
    delay(100);
    digitalWrite(ledMid, HIGH);
    delay(100);
    digitalWrite(ledRight, LOW);
    delay(100);
    digitalWrite(ledMid, LOW);
}

void allLightsOff() {
    digitalWrite(ledLeft, LOW);
    digitalWrite(ledMid, LOW);
    digitalWrite(ledRight, LOW);
}

// -------------------------------------------------------------
// MOVEMENT FUNCTIONS
// -------------------------------------------------------------

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

void uturn() {
    servoLeft.writeMicroseconds(leftServoStop - 40);
    servoRight.writeMicroseconds(rightServoStop - 44);
    delay(timeToTurnLeft * 1.5);
    stop();
}

void stop() {
    servoLeft.writeMicroseconds(leftServoStop);
    servoRight.writeMicroseconds(rightServoStop);
}

// -------------------------------------------------------------
// CORRECTION FUNCTIONS
// -------------------------------------------------------------

void centreToLeftWall() {
    Serial.println("Centring to left wall");
    bool centred = false;

    while (centred == false) {
        int previousZone = irDistance(irLedLeft, irSensorLeft, 38500, 1000);
        rightTurn15();
        if (irDistance(irLedLeft, irSensorLeft, 38500, 1000) > previousZone) {
            leftTurn15();
            centred = true;
        }
        delay(500);
    }
}

void centreToRightWall() {
    Serial.println("Centring to right wall");
    bool centred = false;

    while (centred == false) {
        int previousZone = irDistance(irLedRight, irSensorRight, 38000, 1000);
        leftTurn15();
        if (irDistance(irLedRight, irSensorRight, 38000, 1000) > previousZone) {
            rightTurn15();
            centred = true;
        }
        delay(500);
    }
}

void centreAtStart() {
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
}