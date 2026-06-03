int btn1 = 5;
int led1 = 2;
int btn2 = 9;
int led2 = 6;
int bz = 10;
int startLed = 13;

void setup(){
  pinMode(btn1, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(led2, OUTPUT);
  pinMode(bz, OUTPUT);
  pinMode(startLed, OUTPUT);
  randomSeed(analogRead(0));
}

void loop(){
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(startLed, LOW);

  delay(random(2000, 5000));

  digitalWrite(startLed, LOW);

  while (true) {
    if (digitalRead(btn1) == LOW) {
      digitalWrite(led1, HIGH);
      tone(bz, 1000, 500);
      while(true);
    }
    if ( digitalRead(btn2) == LOW) {
      digitalWrite(led2, HIGH);
      tone(bz, 1000, 500);
      while(true);
    }
  }
}
 
  
