#include <LiquidCrystal.h>
#include <dht.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
dht DHT;

void setup(){
  lcd.begin(16, 2);
}

void loop(){
  DHT.read11(7);

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print((int)DHT.temperature);
  lcd.print("C ");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print((int)DHT.humidity);
  lcd.print("% ");

  delay(2000);
}
