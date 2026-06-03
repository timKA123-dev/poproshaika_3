void setup(){
  for (int i = 2; i <= 5; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

void loop(){

  digitalWrite(2, HIGH); delay(20); digitalWrite(2, LOW);
  digitalWrite(4, HIGH); delay(20); digitalWrite(4, LOW);
  digitalWrite(3, HIGH); delay(20); digitalWrite(3, LOW);
  digitalWrite(5, HIGH); delay(20); digitalWrite(5, LOW);
}
