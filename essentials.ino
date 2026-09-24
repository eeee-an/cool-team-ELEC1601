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
}

void loop() {

    delay(10000);
    seizure();


}

int irDistance(int irLedPin, int irSensorPin, long intercept, long increment) {
    int distance = 0;
    //for (long f = intercept; f)


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

void seizure() {
    servoLeft.writeMicroseconds(leftServoStop - 150);
    servoRight.writeMicroseconds(rightServoStop - 150);
    delay(100);
    servoLeft.writeMicroseconds(leftServoStop + 150);
    servoRight.writeMicroseconds(rightServoStop + 150);
    delay(100);
    servoLeft.writeMicroseconds(leftServoStop + 150);
    servoRight.writeMicroseconds(rightServoStop - 150);
    delay(100);
    servoLeft.writeMicroseconds(leftServoStop - 150);
    servoRight.writeMicroseconds(rightServoStop + 150);
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

void stop() {
    servoLeft.writeMicroseconds(1500);
    servoRight.writeMicroseconds(1490);
}

int forwardAndCount() {
    int count = 0;
    while (irDetect(irLedMid, irSensorMid, 38000) == 1) {
        servoLeft.writeMicroseconds(leftServoStop + 40);
        servoRight.writeMicroseconds(rightServoStop - 40);
        count++;
        delay(100);
    }
    stop();
    Serial.println("Count: " + String(count));
    return count;
}