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

const int timeToTurnLeft = 1500; //1250
const int timeToTurnRight = 1346; //1050

int finalEnd = false;

Servo servoLeft;
Servo servoRight;


//const int wideTurn = 40;
//const int narrowTurn = 20;

//need: starting code, go forward until either front is detected or L and R change
// left turn (perfect)
// right turn (perfect)
// dead end (persistent variable to make sure only one dead end has been reached)
// 180 degree turn (perfect)

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
        leftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
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
    showLights(0, 0, 0);

    if (leftBlocked && rightBlocked) {

            if (!frontBlocked) {
                // 001: Middle of long corridor
                Serial.println("forward case");
                showLights(0, 1, 0);
                delay(1000);
                forwardLoop();
                stopWithLights();
            } else {
                // 100: Dead End
                Serial.println("100: Dead End");
                showLights(1, 0, 1);
                delay(1000);
                if (finalEnd) {
                    showLights(1, 1, 1);
                    delay(3000000);
                }
                //insert if statement here to see if it's the first dead end
                finalEnd = true;
                goBackOne();
                uturn();

                delay(500);
         
                goForwardOne();
                stopWithLights();
            }
             Serial.println("Corridor parallel adjustments");
        int adjustments = 0;
        
        // Loop while difference is > 1 and adjustments < 6
        // old parallel adjustment
        /*
        while (abs(rightDistance - leftDistance) > 2 && adjustments < 4) {
            leftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
            rightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
            if (leftDistance < rightDistance) {
                // 101: Bad start, left parallel
                Serial.println("101: Bad start, left parallel");
                showLights(1, 0, 1);
                rightTurn15();
                delay(500);
                goForwardOne();
                delay(500);
                leftTurn15();
                delay(500);
                goBackOne();
                adjustments++;
            } else {
                // 110: Bad start, right parallel
                Serial.println("110: Bad start, right parallel");
                showLights(1, 1, 0);
                leftTurn15();
                delay(500);
                goForwardOne();
                delay(500);
                rightTurn15();
                delay(500);
                goBackOne();
                adjustments++;
            }
            // Re-check distances for the while loop
            leftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
            rightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
            delay(500);
        }*/

    } else if (leftBlocked && frontBlocked && !rightBlocked) {
            Serial.println("LEFT and FRONT blocked");
            rightTurn30();
            delay(500);

            int newRightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
            int newRightClear = irDetect(irLedRight, irSensorRight, 38000);
            
            if (newRightClear == 1) {
                // 011: Ideal right turn
                Serial.println("011: Ideal right turn");

                reverseRightTurn30(); // reset angle
                goCloseToWall();
                delay(500);
                rightTurn90();
                delay(500);
                goForwardFive();
   
                stopWithLights();
            } else if (1==1) {
                // 111: Bad start, angled left
                Serial.println("111: Bad start, angled left");

 // reset angle
                newRightDistance = irDistance(irLedRight, irSensorRight, 38000, 300);
                rightTurn5();
                int extraNewRightDistance = irDistance(irLedRight, irSensorRight, 38000, 300);
                while (extraNewRightDistance <= newRightDistance) {

                
                rightTurn1();
                delay(300);
                newRightDistance = extraNewRightDistance;
                extraNewRightDistance = irDistance(irLedRight, irSensorRight, 38000, 300);
                }
                leftTurn1();

                goForwardThree();
                delay(500);
                //centreToRightWall();
                stop();
        }
    }
    else if (rightBlocked && frontBlocked && !leftBlocked) {
        Serial.println("RIGHT and FRONT blocked");
        leftTurn30();
        delay(500);

        int newLeftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
        int newLeftClear = irDetect(irLedLeft, irSensorLeft, 38000);
        
        if (newLeftClear == 1) {
            // 010: Ideal left turn
            Serial.println("010: Ideal left turn");

            reverseLeftTurn30(); // reset angle
            goCloseToWall();
            delay(500);
            leftTurn90();
            delay(500);
            goForwardFive();
       
            stopWithLights();
        } else if (1==1) {
            // 001: Bad start, angled right
   
            
            newLeftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 300);
            leftTurn5();
            int extraNewLeftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 300);
            
            while (extraNewLeftDistance <= newLeftDistance) {

                
                leftTurn1();
                delay(300);
                newLeftDistance = extraNewLeftDistance;
                extraNewLeftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 300);
            }
            rightTurn1();
            goForwardThree();
            //centreToLeftWall();
            stopWithLights();
        }
    } else {
        goForwardThree();
    }

}

void goForwardFive() {
    servoLeft.writeMicroseconds(leftServoStop + 40);
    servoRight.writeMicroseconds(rightServoStop - 41);
    delay(1500);
    stop();
}

void goForwardThree() {
    servoLeft.writeMicroseconds(leftServoStop + 50);
    servoRight.writeMicroseconds(rightServoStop - 51);
    delay(700);
    stop();
}

void goForwardOne() {
    servoLeft.writeMicroseconds(leftServoStop + 40);
    servoRight.writeMicroseconds(rightServoStop - 41);
    delay(450);
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

void goBackOne() {
    servoLeft.writeMicroseconds(leftServoStop - 41);
    servoRight.writeMicroseconds(rightServoStop + 40);
    delay(300);
    stop();
}

void forwardLoop() {
    int frontClear = irDetect(irLedMid, irSensorMid, 38000);
    
    
    while (frontClear == 1) {
        
        
        int leftDist = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
        int rightDist = irDistance(irLedRight, irSensorRight, 38000, 1000);
        int leftClear = irDetect(irLedLeft, irSensorLeft, 38000);
        int rightClear = irDetect(irLedRight, irSensorRight, 38000);
        
        // hardcoded speed offsets
        servoLeft.writeMicroseconds(leftServoStop + 50);
        servoRight.writeMicroseconds(rightServoStop - 51);
        showLights(1, 0, 1);
        if (rightClear == 0 && leftClear == 0) {
            
        
        if (leftDist < (rightDist - 2)) {
            showLights(1, 0, 0);
            servoLeft.writeMicroseconds(leftServoStop + 60);
            servoRight.writeMicroseconds(rightServoStop - 30);
        } 
        else if (rightDist < (leftDist - 2)) {
            showLights(0, 0, 1);
            servoLeft.writeMicroseconds(leftServoStop + 30);
            servoRight.writeMicroseconds(rightServoStop - 60);
        }
    }
        
        delay(50);
        showLights(0, 1, 0);
   
        frontClear = irDetect(irLedMid, irSensorMid, 38000);
    }
    
    // Once the loop breaks (front is blocked), inch closer to the wall safely
    goCloseToWall();
}

void goCloseToWall() {
    int tempFrontDistance = irDistance(irLedMid, irSensorMid, 38000, 1000);
    while (tempFrontDistance > 3) {
        goForwardOne();
        tempFrontDistance = irDistance(irLedMid, irSensorMid, 38000, 1000);
    }
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
    delay(timeToTurnLeft / 15);
    stop();
}

void rightTurn5() {
    servoLeft.writeMicroseconds(leftServoStop + 43);
    servoRight.writeMicroseconds(rightServoStop + 21);
    delay(timeToTurnRight / 15);
    stop();
}

void leftTurn1() {
    
    servoLeft.writeMicroseconds(leftServoStop - 20);
    servoRight.writeMicroseconds(rightServoStop - 44);
    delay(timeToTurnLeft / 22);
    stop();
}

void rightTurn1() {
    servoLeft.writeMicroseconds(leftServoStop + 43);
    servoRight.writeMicroseconds(rightServoStop + 21);
    delay(timeToTurnRight / 22);
    stop();
}
/*
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
*/

void uturn() {
    servoLeft.writeMicroseconds(leftServoStop - 40);
    servoRight.writeMicroseconds(rightServoStop - 44);
    delay(timeToTurnLeft * 1.42); 
    stop();
}

void stop() {
    servoLeft.writeMicroseconds(leftServoStop);
    servoRight.writeMicroseconds(rightServoStop);
}

int irDistance(int irLedPin, int irSensorPin, long intercept, long increment) {
   int distance = 0;
   for(long frequency = intercept; frequency <= 45000; frequency += increment)
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

void stopWithLights() {
    stop();
    // flashes 3 times why not
    for(int i = 0; i < 3; i++) {
        digitalWrite(ledLeft, HIGH);
        digitalWrite(ledRight, HIGH);
        delay(100);
        digitalWrite(ledLeft, LOW);
        digitalWrite(ledRight, LOW);
        delay(100);
    }
}

void tryBecomeParallel() {
    int newLeftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 300);
    leftTurn1();
    int extraNewLeftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 300);
            while (extraNewLeftDistance <= newLeftDistance) {

                
                leftTurn1();
                delay(300);
                newLeftDistance = extraNewLeftDistance;
                extraNewLeftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 300);
            }
            leftTurn1();
            leftTurn1();



     int newRightDistance = irDistance(irLedRight, irSensorRight, 38000, 300);
    rightTurn1();
    int extraNewRightDistance = irDistance(irLedRight, irSensorRight, 38000, 300);
            while (extraNewRightDistance <= newRightDistance) {

                
                rightTurn1();
                delay(300);
                newRightDistance = extraNewRightDistance;
                extraNewRightDistance = irDistance(irLedRight, irSensorRight, 38000, 300);
            }
            rightTurn1();
            rightTurn1();
            
            goForwardThree();
            //centreToLeftWall();
            stopWithLights();
}
