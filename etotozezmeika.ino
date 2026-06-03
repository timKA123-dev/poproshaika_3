#include <LiquidCrystal.h>

// Инициализация дисплея LiquidCrystal(RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 

// Джойстик
const int joyX = A0;
const int joyY = A1;
const int joyBtn = 8;

// Зуммер
const int buzzer = 9;

// Параметры змейки
int snakeX[32], snakeY[32]; // Уменьшено до 32 для экономии памяти Arduino
int snakeLen = 3;
int dirX = 1, dirY = 0;
int appleX, appleY;
int score = 0;
bool gameOver = false;

unsigned long lastMove = 0;
int moveDelay = 300; // Теперь это обычная переменная для ускорения игры

// Кастомный символ для тела змейки (квадрат)
byte snakeBody[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void setup() {
  lcd.begin(16, 2);
  pinMode(joyBtn, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  randomSeed(analogRead(2)); // Используем неподключенный пин для генерации случайных чисел
  
  lcd.createChar(0, snakeBody); // Загружаем символ в память дисплея
  startGame();
}

void startGame() {
  snakeLen = 3;
  dirX = 1; dirY = 0;
  snakeX[0] = 7; snakeY[0] = 3; // Изначально координаты за пределами экрана или неверны для 16x2?
  snakeX[0] = 5; snakeY[0] = 0; // Исправлено: начальные координаты на экране 16x2
  snakeX[1] = 4; snakeY[1] = 0;
  snakeX[2] = 3; snakeY[2] = 0;
  
  placeApple();
  score = 0;
  moveDelay = 300; // Сброс скорости
  gameOver = false;
  lcd.clear();
}

void placeApple() {
  appleX = random(16);
  appleY = random(2);
}

void readJoystick() {
  int x = analogRead(joyX);
  int y = analogRead(joyY);
  
  // Условие (dirX != -1) предотвращает движение змейки "внутрь себя"
  if (x > 700 && dirX != -1) { dirX = 1;  dirY = 0; }
  if (x < 300 && dirX != 1)  { dirX = -1; dirY = 0; }
  if (y > 700 && dirY != -1) { dirY = 1;  dirX = 0; }
  if (y < 300 && dirY != 1)  { dirY = -1; dirX = 0; }
}

void moveSnake() {
  // 1. Двигаем тело
  for (int i = snakeLen - 1; i > 0; i--) {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
  }
  // 2. Двигаем голову
  snakeX[0] += dirX;
  snakeY[0] += dirY;

  // 3. Проверка выхода за границы экрана
  if (snakeX[0] < 0 || snakeX[0] >= 16 || snakeY[0] < 0 || snakeY[0] >= 2) {
    gameOver = true;
    tone(buzzer, 300, 500);
    return;
  }

  // 4. Проверка поедания яблока
  if (snakeX[0] == appleX && snakeY[0] == appleY) {
    snakeLen++;
    score++;
    tone(buzzer, 1000, 50);
    placeApple();
    
    // Увеличение скорости игры (уменьшение задержки)
    if (moveDelay > 100) {
      moveDelay -= 20;
    }
    return;
  }

  // 5. Проверка столкновений с собственным телом
  for (int i = snakeLen - 1; i > 0; i--) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true;
      tone(buzzer, 300, 500);
    }
  }
}

void draw() {
  lcd.clear(); // Очищаем экран в начале кадра (избегаем артефактов)
  
  // Отрисовка змейки
  for (int i = snakeLen - 1; i >= 0; i--) {
    lcd.setCursor(snakeX[i], snakeY[i]);
    if (i == 0) {
      lcd.write(byte(0)); // Голова (тот же кастомный символ, можно поменять при желании)
    } else {
      lcd.write(byte(0)); // Тело
    }
  }
  
  // Отрисовка яблока
  lcd.setCursor(appleX, appleY);
  lcd.print("O");
  
  // Отрисовка счета
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
}

void loop() {
  if (!gameOver) {
    unsigned long now = millis();
    if (now - lastMove > moveDelay) {
      lastMove = now;
      readJoystick();
      moveSnake();
      if (!gameOver) draw(); // Перерисовываем, если не проиграли в moveSnake
    }
  } else {
    noTone(buzzer);
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("GAME OVER");
    lcd.setCursor(4, 1);
    lcd.print("Score: ");
    lcd.print(score);
    
    delay(2000); // Даем прочитать счет
    
    // Ожидание нажатия кнопки джойстика для возрождения
    while (digitalRead(joyBtn) == HIGH) {} 
    delay(200); // Антидребезг
    startGame();
  }
}
