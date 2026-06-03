#include <Arduino.h>
const int R = 7;
void setup(){
  pinMode(R, OUTPUT);
}
void loop(){
  digitalWrite(R, HIGH);
  delay(2000);
  digitalWrite(R, LOW);
  delay(2000);
}
