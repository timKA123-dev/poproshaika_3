#include <LiquidCrystal.h>

// Подключение LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Пины: Джойстик (VRX, VRY, SW), Зуммер
const int pinVRX = A0, pinVRY = A1, pinSW = 8, pinBuzzer = 9;

// Игровые переменные
const int maxLen = 30;
int snakeX[maxLen], snakeY[maxLen], snakeLen = 3;
int dirX = 1, dirY = 0, foodX, foodY;
bool gameOver = false, gameStarted = false;
unsigned long lastMove = 0;
const int speed = 250;

void setup() {
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinBuzzer, OUTPUT);
  lcd.begin(16, 2);
  randomSeed(analogRead(A2));
  showStart();
}

void loop() {
  if (!gameStarted || gameOver) {
    if (digitalRead(pinSW) == LOW) startGame();
    return;
  }
  
  // Управление
  int xVal = analogRead(pinVRX), yVal = analogRead(pinVRY);
  if (xVal < 300 && dirX == 0) { dirX = -1; dirY = 0; }
  else if (xVal > 700 && dirX == 0) { dirX = 1; dirY = 0; }
  if (yVal < 300 && dirY == 0) { dirX = 0; dirY = -1; }
  else if (yVal > 700 && dirY == 0) { dirX = 0; dirY = 1; }

  if (millis() - lastMove > speed) {
    moveSnake();
    lastMove = millis();
  }
}

void showStart() {
  lcd.clear();
  lcd.print("SNAKE: PRESS SW");
}

void startGame() {
  gameStarted = true; gameOver = false;
  snakeLen = 3;
  for(int i=0; i<3; i++) { snakeX[i] = 5-i; snakeY[i] = 0; }
  dirX = 1; dirY = 0;
  generateFood();
}

void moveSnake() {
  int nextX = snakeX[0] + dirX, nextY = snakeY[0] + dirY;

  // Вертикаль: сквозная
  if (nextY < 0) nextY = 1; else if (nextY > 1) nextY = 0;
  // Горизонталь: граница
  if (nextX < 0 || nextX >= 16) { endGame(); return; }

  // Проверка столкновения с собой
  for (int i = 1; i < snakeLen; i++)
    if (snakeX[i] == nextX && snakeY[i] == nextY) { endGame(); return; }

  // Движение
  for (int i = snakeLen - 1; i > 0; i--) { snakeX[i] = snakeX[i-1]; snakeY[i] = snakeY[i-1]; }
  snakeX[0] = nextX; snakeY[0] = nextY;

  // Еда
  if (nextX == foodX && nextY == foodY) {
    if (snakeLen < maxLen) snakeLen++;
    generateFood();
  }
  draw();
}

void generateFood() {
  foodX = random(0, 16); foodY = random(0, 2);
}

void draw() {
  lcd.clear();
  lcd.setCursor(foodX, foodY); lcd.print("*");
  for (int i = 0; i < snakeLen; i++) {
    lcd.setCursor(snakeX[i], snakeY[i]);
    lcd.print(i == 0 ? "O" : "o");
  }
}

void endGame() {
  gameOver = true;
  lcd.clear();
  lcd.print("GAME OVER");
  tone(pinBuzzer, 1000, 200);
}
