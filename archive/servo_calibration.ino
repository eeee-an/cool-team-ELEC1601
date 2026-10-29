#include <Servo.h>

Servo servoLeft;

void setup() {
    servoLeft.attach(13);
    Serial.begin(9600);  
}

void loop() {
    servoLeft.writeMicroseconds();
    delay(500);
}