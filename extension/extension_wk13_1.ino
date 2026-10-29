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

const int timeToTurnLeft = 1520; //1250 //1500 is exact
const int timeToTurnRight = 1356; //1050 //1346 is exact

const int maxEligibleStraightZone = 5; //maximum eligible zone to keep going straight, could prolly change to 4
const int steerDeadzone = 2; //like a controller deadzone but for the middle of the course to not constantly adjust to go straight
//difference beteween left and right zone


bool finalEnd = false;
bool badStart = true;

bool uTurnSinceLastFork = false;
bool gotThroughFork = false;
int lastForkDecision = 0; //0 is forward, 1 is turn

int longCorridoorCount = 0;

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
    //delay(200);
    //showLights(0, 0, 0);

    if (badStart) {
        int newRightDistance = irDistance(irLedRight, irSensorRight, 38000, 300);
        int newLeftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 300);
    //while (badStart) {//incomplete while loop
        if (leftBlocked && !rightBlocked) {
            Serial.println("111: Bad start, angled left");

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
            delay(500);
            stop();
        } else if (rightBlocked && !leftBlocked) {
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
            delay(500);
            //centreToLeftWall();
            stop();
        } else {
            //ideally run centreing
            badStart = false;
        }
        badStart = false;
        goStraightAfterTurning();
        return;
    //}
    }

    if (leftBlocked && rightBlocked) {
        if (!frontBlocked) {
            // 001: Middle of long corridor
            Serial.println("forward case");
            showLights(0, 1, 0);
            //delay(1000);
            //forwardLoop();

            int leftDist = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
            int rightDist = irDistance(irLedRight, irSensorRight, 38000, 1000);
            int leftClear = irDetect(irLedLeft, irSensorLeft, 40000); // less sensitive so hopefully it doesnt start turning into corners
            int rightClear = irDetect(irLedRight, irSensorRight, 40000);
                    
            longCorridoorCount++;

            Serial.println(longCorridoorCount);
            servoLeft.writeMicroseconds(leftServoStop + 40);
            servoRight.writeMicroseconds(rightServoStop - 41);
            showLights(1, 0, 1);
            if (rightClear == 0 && leftClear == 0) {
                        
                    
                if (leftDist < (rightDist - 2)) {
                    showLights(1, 0, 0);
                    servoLeft.writeMicroseconds(leftServoStop + 50);
                    servoRight.writeMicroseconds(rightServoStop - 30);
                } 
                else if (rightDist < (leftDist - 2)) {
                    showLights(0, 0, 1);
                    servoLeft.writeMicroseconds(leftServoStop + 30);
                    servoRight.writeMicroseconds(rightServoStop - 50);
                } else {
                    //potential issue here with constantly angling left-right, could have a parallel adjustment function
                    servoLeft.writeMicroseconds(leftServoStop + 40);
                    servoRight.writeMicroseconds(rightServoStop - 41);

                }
            } else {
                //this condition happens if theres walls far away on both sides but one or both is far enough to potentially be a turn instead
                //hopefully just going straight if a turn comes up and cooks the going straight zones
                servoLeft.writeMicroseconds(leftServoStop + 40);
                servoRight.writeMicroseconds(rightServoStop - 41);
                delay(300);
            }
                            
        } else if (midDistance < 4)  {
            // 100: Dead End
            stop();
            Serial.println("100: Dead End");
            showLights(1, 0, 1);
            delay(1000);
            if (longCorridoorCount > 35) {
                showLights(1, 1, 1);
                delay(3000000);
                longCorridoorCount = 0;
            }
            //insert if statement here to see if it's the first dead end for ext 2
            //finalEnd = true;
            //not using final end because we're using the long dead end check instead, which is in the uturn function
            goBackward(300);
            uturn(); 
            stop();
        } else {
            showLights (1, 1, 0);
            //idk
            goForward(200);
        }



    } else if (leftBlocked && frontBlocked && !rightBlocked) {
        stop();
        longCorridoorCount = 0;
        Serial.println("LEFT and FRONT blocked");
        rightTurn30();
        delay(200);

        int newRightDistance = irDistance(irLedRight, irSensorRight, 38000, 1000);
        int newRightClear = irDetect(irLedRight, irSensorRight, 38000);
            
        if (newRightClear == 1) {
            // 011: Ideal right turn
            Serial.println("011: Ideal right turn");

            reverseRightTurn30(); // reset angle
            goCloseToWall();
            delay(300);
            rightTurn90();
            delay(300);
            goStraightAfterTurning();
            stop();

        } else if (1==1) {
            //weird case, most likely angled 45 degrees to a wall
            for (int j = 0; j < 10; j++) {
                showLights(1, 1, 1);
                delay(100);
                showLights(0, 0, 0);
                delay(100);
            }
        }
    }
    else if (rightBlocked && frontBlocked && !leftBlocked) {
        longCorridoorCount = 0;
        stop();
        Serial.println("RIGHT and FRONT blocked");
        leftTurn30();
        delay(200);

        int newLeftDistance = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
        int newLeftClear = irDetect(irLedLeft, irSensorLeft, 38000);
        
        if (newLeftClear == 1) {
            // 010: Ideal left turn
            Serial.println("010: Ideal left turn");

            reverseLeftTurn30(); // reset angle
            goCloseToWall();
            delay(300);
            leftTurn90();
            delay(300);
            goStraightAfterTurning();
            stop();

        } else if (1==1) {
            //cooked case, probably angled right 45 degrees

         
        }
    } else if (frontBlocked && !leftBlocked && !rightBlocked) {
        longCorridoorCount = 0;
        // T intersection
        stop();
        goCloseToWall();
        goBackward(200);
        leftTurn90();
 
        //for the forks, there should also be an if for accidentally detecting a fork when there's a turn instead.
        //commented out for ext pt 1
        
    } else if (leftBlocked && !frontBlocked && !rightBlocked) {
        longCorridoorCount = 0;
        stop();
        //fork with option forward or right
        if (gotThroughFork) {
            goStraightAfterTurning();
        } else if (lastForkDecision == 0) {
            goForward(1100);
            rightTurn90();
            gotThroughFork = true;
            lastForkDecision = 1;
        }
    } else if (rightBlocked && !frontBlocked && !leftBlocked) {
        longCorridoorCount = 0;
        stop();
        if (gotThroughFork) {
            goStraightAfterTurning();
        } else if (lastForkDecision == 0) {
            goForward(1100);
            leftTurn90();
            gotThroughFork = true;
            lastForkDecision = 1;
        }
        //nothing at the moment idk

    } else {
        //no clue what happened but just go forward
        showLights(0, 1, 0);
        goForward(300);
    }


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

void goForward(int time) {
    servoLeft.writeMicroseconds(leftServoStop + 40);
    servoRight.writeMicroseconds(rightServoStop - 41);
    delay(time);
    stop();
}

void goBackward(int time) {
    servoLeft.writeMicroseconds(leftServoStop - 41);
    servoRight.writeMicroseconds(rightServoStop + 40);
    delay(time);
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
        servoLeft.writeMicroseconds(leftServoStop + 40);
        servoRight.writeMicroseconds(rightServoStop - 41);
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
        goForward(100);
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
    delay(timeToTurnLeft / 3.3);
    stop();
}

void reverseRightTurn30() {
    servoLeft.writeMicroseconds(leftServoStop - 43);
    servoRight.writeMicroseconds(rightServoStop - 21);
    delay(timeToTurnRight / 2.9);
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

void uturn() {
    if (longCorridoorCount > 20) {
        delay(5000000);
        //long end!
    }
    servoLeft.writeMicroseconds(leftServoStop - 40);
    servoRight.writeMicroseconds(rightServoStop - 44);
    delay(timeToTurnLeft * 1.425); 
    stop();
}

void stop() {
    servoLeft.writeMicroseconds(leftServoStop);
    servoRight.writeMicroseconds(rightServoStop);
}

void smoothStop() {
    servoLeft.writeMicroseconds(leftServoStop + 20);
    servoRight.writeMicroseconds(rightServoStop - 20);
    delay(100);
    servoLeft.writeMicroseconds(leftServoStop + 10);
    servoRight.writeMicroseconds(rightServoStop - 10);
    delay(100);
    servoLeft.writeMicroseconds(leftServoStop);
    servoRight.writeMicroseconds(rightServoStop);
}

void showLights(int l, int m, int r) {
    digitalWrite(ledLeft, l);
    digitalWrite(ledMid, m);
    digitalWrite(ledRight, r);
}

void goStraightAfterTurning() {
    for (int h = 0; h < 5; h++) {
    int leftDist = irDistance(irLedLeft, irSensorLeft, 38000, 1000);
    int rightDist = irDistance(irLedRight, irSensorRight, 38000, 1000);
    int leftClear = irDetect(irLedLeft, irSensorLeft, 40000); // less sensitive so hopefully it doesnt start turning into corners
    int rightClear = irDetect(irLedRight, irSensorRight, 40000);
                    

    servoLeft.writeMicroseconds(leftServoStop + 40);
    servoRight.writeMicroseconds(rightServoStop - 41);
    showLights(0, 0, 1);
    if (rightClear == 0 && leftClear == 0) {
                        
                    
        if (leftDist < (rightDist - 2)) {
            showLights(1, 0, 0);
            servoLeft.writeMicroseconds(leftServoStop + 50);
            servoRight.writeMicroseconds(rightServoStop - 20);
        } 
        else if (rightDist < (leftDist - 2)) {
            showLights(0, 0, 1);
            servoLeft.writeMicroseconds(leftServoStop + 20);
            servoRight.writeMicroseconds(rightServoStop - 50);
        } else {
            //straight enough
            servoLeft.writeMicroseconds(leftServoStop + 40);
            servoRight.writeMicroseconds(rightServoStop - 41);

        }
        delay(50);
        //these next conditions assume that after turning, sometimes the car's angle is pretty bad and doesnt have reference walls to go off of
    } else if (rightClear = 1) {
        showLights(0, 0, 0);
        servoLeft.writeMicroseconds(leftServoStop + 50);
        servoRight.writeMicroseconds(rightServoStop - 20);//this doesnt do anything lol it gets overriden
        goForward(200);
    } else if (leftClear = 1) {
        showLights(0, 0, 0);
        servoLeft.writeMicroseconds(leftServoStop + 20);
        servoRight.writeMicroseconds(rightServoStop - 50);
        goForward(200);
    } else {
        //no reference walls at all which is weird
        goForward(200);
    }
    }
}