#include <Stepper.h>
Stepper m(2048, 2, 4, 3, 5);
void setup(){
  m.setSpeed(10);
}
void loop(){
  m.step(512);
  delay(1000);
  m.step(-512);
  delay(1000);
}
