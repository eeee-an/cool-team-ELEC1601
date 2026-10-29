#include <Servo.h>                      // Include servo library

Servo servoLeft;                        // Declare left servo
Servo servoRight;



void setup()                            // Built in initialization block
{
    servoLeft.attach(13);               // Attach left signal to pin 13
  	servoRight.attach(12);
  
  Serial.begin(9600);
  servoRight.writeMicroseconds(1460);  // 1.3ms = full speed clockwise
  servoLeft.writeMicroseconds(1540);
  delay(5000); //initial foraward to set sensors
  

}

void loop()                             // Main loop auto-repeats
{          
  if ((analogRead(A0) > 238) && (analogRead(A1) > 238)) {
      servoRight.writeMicroseconds(1460);  // 1.3ms = full speed clockwise
  servoLeft.writeMicroseconds(1540);
  }
  delay(200);
  

  if ((analogRead(A1) > 238) && (analogRead(A0) < 238)) {

      servoLeft.writeMicroseconds(1490);  // 1.3ms = full speed clockwise
  servoRight.writeMicroseconds(1490);
  Serial.println("left for 3 seconds");
  delay(3000);
  }
  
  if ((analogRead(A0) > 238) && (analogRead(A1) < 238)) {
      servoLeft.writeMicroseconds(1510);  // 1.3ms = full speed clockwise
  servoRight.writeMicroseconds(1510);
  Serial.println("Right for 3 seconds");
  delay(3000);
  }
  
  if ((analogRead(A0) < 238) && (analogRead(A1) < 238)) {
     servoLeft.writeMicroseconds(1510);  // 1.3ms = full speed clockwise
  servoRight.writeMicroseconds(1510);
  Serial.println("Right for 3 seconds - no walls on either side");
  delay(3000);
       servoRight.writeMicroseconds(1460);  // 1.3ms = full speed clockwise
  servoLeft.writeMicroseconds(1540);
    Serial.println("Forward so it doesn't stay in an endless loop of circling clockwise");
    delay(3000);
    
  }
  
  
  Serial.println(analogRead(A0));
  
  
  //forward, backward, left, right
}