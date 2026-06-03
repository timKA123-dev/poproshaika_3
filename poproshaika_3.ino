#include <Servo.h>
const int buttonPin = 2, buzzerPin = 8, servoPin = 9;
Servo myServo;
void setup(){
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  myServo.attach(servoPin);
  myServo.write(0);
}
void loop(){
  if (digitalRead(buttonPin) == LOW){
    delay(200);
    for (int pos = 0; pos <= 120; pos += 2) { myServo.write(pos); delay(15); }
    delay(1000);
    for (int pos = 120; pos >= 0; pos -= 2) { myServo.write(pos); delay(15); }
    delay(500);
    tone(buzzerPin, 1000, 300);
    delay(2000);
  }
}
