#include <LiquidCrystal.h>

// --- ПИНЫ УПРАВЛЕНИЯ ---
const int pinRS = 12;
const int pinE = 11;
const int pinD4 = 5;
const int pinD5 = 4;
const int pinD6 = 3;
const int pinD7 = 2;
LiquidCrystal lcd(pinRS, pinE, pinD4, pinD5, pinD6, pinD7);

const int buzzerPin = 8;
const int joyX = A0; 
const int joyY = A1; 

// --- ПАРАМЕТРЫ ИГРЫ ---
const int mapWidth = 16;
const int mapHeight = 2;

int snakeX[32];
int snakeY[32];
int snakeLength = 3;

int dirX = 1; // 1 - вправо, -1 - влево, 0 - по вертикали
int dirY = 0; // 1 - вниз, -1 - вверх, 0 - по горизонтали

int appleX;
int appleY;
bool gameOver = false;
unsigned long lastMoveTime = 0;
int gameSpeed = 300; // задержка в мс (чем меньше, тем быстрее)

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  
  lcd.begin(mapWidth, mapHeight);
  randomSeed(analogRead(A2));
  
  startGame();
}

void startGame() {
  snakeLength = 3;
  snakeX[0] = 5; snakeY[0] = 1;
  snakeX[1] = 4; snakeY[1] = 1;
  snakeX[2] = 3; snakeY[2] = 1;
  
  dirX = 1;
  dirY = 0;
  gameOver = false;
  
  spawnApple();
  drawBoard();
}

void spawnApple() {
  bool onSnake = true;
  while(onSnake) {
    appleX = random(0, mapWidth);
    appleY = random(0, mapHeight);
    onSnake = false;
    for(int i = 0; i < snakeLength; i++) {
      if(snakeX[i] == appleX && snakeY[i] == appleY) {
        onSnake = true;
      }
    }
  }
}

void loop() {
  if (gameOver) return;

  // --- ЧТЕНИЕ ДЖОЙСТИКА ---
  int xValue = analogRead(joyX);
  int yValue = analogRead(joyY);

  // Считываем повороты только если змейка движется "прямо"
  if (dirX != 0) { // Движемся горизонтально, значит джойстик может менять Y
    if (yValue < 300) { dirX = 0; dirY = -1; } // Вверх
    else if (yValue > 700) { dirX = 0; dirY = 1; }  // Вниз
  } else if (dirY != 0) { // Движемся вертикально, значит джойстик может менять X
    if (xValue < 300) { dirX = 1; dirY = 0; } // Вправо
    else if (xValue > 700) { dirX = -1; dirY = 0; } // Влево
  }

  // --- ЛОГИКА ДВИЖЕНИЯ ---
  if (millis() - lastMoveTime > gameSpeed) {
    lastMoveTime = millis();
    moveSnake();
  }
}

void moveSnake() {
  int newX = snakeX[0] + dirX;
  int newY = snakeY[0] + dirY;

  // --- БЕСКОНЕЧНЫЕ ВЕРТИКАЛЬНЫЕ СТЕНЫ ---
  if (newX >= mapWidth) newX = 0;
  else if (newX < 0) newX = mapWidth - 1;

  // --- СТОЛКНОВЕНИЕ С ГОРИЗОНТАЛЬНЫМИ СТЕНАМИ ---
  if (newY < 0 || newY >= mapHeight) {
    dieSequence();
    return;
  }

  // --- СТОЛКНОВЕНИЕ С СОБОЙ ---
  for(int i = 0; i < snakeLength; i++) {
    if(snakeX[i] == newX && snakeY[i] == newY) {
      dieSequence();
      return;
    }
  }

  // --- ЕДА И ХВОСТ ---
  bool ateApple = (newX == appleX && newY == appleY);

  // Сдвигаем тело
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
  }
  
  snakeX[0] = newX;
  snakeY[0] = newY;

  if (ateApple) {
    snakeLength++;
    tone(buzzerPin, 4000, 50); // Короткий писк
    spawnApple();
  }

  drawBoard();
}

void drawBoard() {
  lcd.clear();
  // Рисуем еду
  lcd.setCursor(appleX, appleY);
  lcd.print("*");
  
  // Рисуем змейку
  for(int i = 0; i < snakeLength; i++) {
    lcd.setCursor(snakeX[i], snakeY[i]);
    if (i == 0) lcd.print("O"); // Голова
    else lcd.print("o"); // Тело
  }
}

void dieSequence() {
  gameOver = true;
  // Длинный писк при проигрыше
  tone(buzzerPin, 1000, 800);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("GAME OVER");
  
  // Ожидание (чтобы не перезапустить моментально, если зажат джойстик)
  delay(1000); 
  
  // Ожидание действия (взмах джойстика) для перезапуска
  while(analogRead(joyX) > 400 && analogRead(joyX) < 600 && 
        analogRead(joyY) > 400 && analogRead(joyY) < 600) {
     // Ждем шевеления джойстика
  }
  startGame();
}
