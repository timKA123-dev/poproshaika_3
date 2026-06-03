#include <SPI.h>
#include <Servo.h>

// Определение пинов для подключения (UNO/NANO)
#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 6
#define BUZZER_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN); // Создание экземпляра MFRC522
Servo myServo;                    // Создание объекта Servo

// 1. ЗАМЕНИТЕ ЭТИ ЗНАЧЕНИЯ НА UID ВАШЕЙ КАРТЫ, КОТОРЫЙ ВЫВЕДЕТСЯ В СЕРИАЛ
byte authorizedUID[] = {0xDE, 0xAD, 0xBE, 0xEF}; 

void setup() {
  Serial.begin(9600);
  while (!Serial); // Ожидание открытия порта
  SPI.begin();
  mfrc522.PCD_Init();
  
  myServo.attach(SERVO_PIN);
  myServo.write(0); // Закрытое положение (0 градусов)
  
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("Ожидание карты...");
}

void loop() {
  // Поиск новой карты
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID метки:");
  bool accessGranted = true;
  
  // Проверка UID (сравнение с authorizedUID)
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    
    if (mfrc522.uid.uidByte[i] != authorizedUID[i]) {
      accessGranted = false;
    }
  }
  Serial.println();

  if (accessGranted) {
    Serial.println("Доступ разрешен");
    // Звук одобрения
    tone(BUZZER_PIN, 1000, 200);
    delay(200);
    tone(BUZZER_PIN, 1500, 200);
    
    myServo.write(90); // Открыть (90 градусов)
    delay(3000);       // Ждать 3 секунды
    myServo.write(0);  // Закрыть
  } else {
    Serial.println("Доступ запрещен");
    // Звук ошибки
    tone(BUZZER_PIN, 300, 500); 
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
