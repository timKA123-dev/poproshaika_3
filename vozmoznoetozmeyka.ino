#include <LiquidCrystal.h>
#include <EEPROM.h>

// Подключение LCD 1602 без I2C
const int pinRS = 12, pinEN = 11, pinD4 = 5, pinD5 = 4, pinD6 = 3, pinD7 = 2;
LiquidCrystal lcd(pinRS, pinEN, pinD4, pinD5, pinD6, pinD7);

const int buzzerPin = 8;
const int joyX = A0;
const int joyY = A1;
const int joySw = 3;

// Игровое поле: физически 16 символов, по 2 строки. 
// Программно разбиваем на 4 пиксельные строки для поля 16х4
#define COLS 16
#define ROWS 4
#define MAX_TAIL 60

int snakeX[MAX_TAIL];
int snakeY[MAX_TAIL];
int snakeLength = 3;
int dirX = 1, dirY = 0;
int foodX, foodY;
int score = 0;
int highScore = 0;
unsigned long lastMoveTime = 0;
unsigned long gameSpeed = 200; // Скорость игры, мс

bool gameOver = false;
bool gameStarted = false;

// 8 кастомных символов для создания графики внутри знакомест 5х8
byte customChars[8][8] = {
  {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}, // 0: Верхняя половина блока
  {B11111, B11111, B00000, B00000, B00000, B00000, B00000, B00000}, // 1: Пусто, но нужна инициализация
  {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111}, // 2: Нижняя половина блока
  {B00000, B01110, B11111, B11111, B11111, B11111, B01110, B00000}, // 3: Еда (яблоко)
  {B11111, B11111, B11111, B11111, B11111, B00000, B00000, B00000}, // 4: Голова вверх
  {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}, // 5: Голова вниз
  {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}, // 6: Голова влево
  {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}  // 7: Голова вправо
};

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(joySw, INPUT_PULLUP);
  
  // Инициализация LCD
  lcd.begin(16, 2);
  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, customChars[i]);
  }

  // Чтение рекорда из EEPROM
  EEPROM.get(0, highScore);
  if(highScore > 9999 || highScore < 0) highScore = 0;

  drawStartScreen();
}

void drawStartScreen() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("SNAKE ARDUINO");
  lcd.setCursor(3, 1);
  lcd.print("Press Key");
}

void startGame() {
  snakeLength = 3;
  snakeX[0] = 5; snakeY[0] = 2;
  snakeX[1] = 4; snakeY[1] = 2;
  snakeX[2] = 3; snakeY[2] = 2;
  dirX = 1; dirY = 0;
  score = 0;
  gameSpeed = 200;
  gameOver = false;
  
  spawnFood();
  lcd.clear();
}

void loop() {
  if (!gameStarted) {
    if (digitalRead(joySw) == LOW) {
      playTone(1000, 50);
      gameStarted = true;
      startGame();
      delay(300);
    }
    return;
  }

  if (gameOver) {
    handleGameOver();
    return;
  }

  handleJoystick();

  if (millis() - lastMoveTime >= gameSpeed) {
    moveSnake();
    lastMoveTime = millis();
  }

  drawGame();
}

void handleJoystick() {
  int xVal = analogRead(joyX);
  int yVal = analogRead(joyY);

  if (xVal < 200 && dirX != 1) { dirX = -1; dirY = 0; }
  else if (xVal > 800 && dirX != -1) { dirX = 1; dirY = 0; }
  else if (yVal < 200 && dirY != 1) { dirX = 0; dirY = -1; }
  else if (yVal > 800 && dirY != -1) { dirX = 0; dirY = 1; }
}

void moveSnake() {
  int nextX = snakeX[0] + dirX;
  int nextY = snakeY[0] + dirY;

  // Прохождение сквозь вертикальные стены
  if (nextX >= COLS) nextX = 0;
  else if (nextX < 0) nextX = COLS - 1;

  // Поведение при горизонтальных стенах (смерть)
  if (nextY >= ROWS || nextY < 0) {
    triggerGameOver();
    return;
  }

  // Самопоедание
  for (int i = 0; i < snakeLength; i++) {
    if (snakeX[i] == nextX && snakeY[i] == nextY) {
      triggerGameOver();
      return;
    }
  }

  // Движение хвоста
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  snakeX[0] = nextX;
  snakeY[0] = nextY;

  // Поедание еды
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    playTone(2000, 100);
    score += 10;
    snakeLength++;
    if (snakeLength > MAX_TAIL) snakeLength = MAX_TAIL;
    if (gameSpeed > 50) gameSpeed -= 10;
    spawnFood();
  }
}

void spawnFood() {
  bool valid = false;
  while (!valid) {
    foodX = random(0, COLS);
    foodY = random(0, ROWS);
    valid = true;
    for (int i = 0; i < snakeLength; i++) {
      if (snakeX[i] == foodX && snakeY[i] == foodY) valid = false;
    }
  }
}

void triggerGameOver() {
  gameOver = true;
  playTone(500, 500);
  
  if (score > highScore) {
    highScore = score;
    EEPROM.put(0, highScore);
  }
}

void handleGameOver() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("GAME OVER");
  lcd.setCursor(3, 1);
  lcd.print("Score: ");
  lcd.print(score);
  delay(2000);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("High Score:");
  lcd.setCursor(6, 1);
  lcd.print(highScore);
  delay(2000);

  gameStarted = false;
  gameOver = false;
  drawStartScreen();
}

void drawGame() {
  // Очистка перед отрисовкой
  char buffer[17];
  
  for(int r = 0; r < 2; r++) {
    lcd.setCursor(0, r);
    for(int c = 0; c < 16; c++) {
      // Заполняем буфер пространствами (символ пробела)
      buffer[c] = ' ';
    }
    buffer[16] = '\0';
    
    // Отрисовка элементов на текущем ряде экрана
    for (int i = 0; i < snakeLength; i++) {
      int cellY = snakeY[i] / 2;
      if (cellY == r) {
        int charRow = snakeY[i] % 2; // 0 (верх), 1 (низ)
        int pos = snakeX[i];
        
        if (cellY == 0 && charRow == 0) {
          // Специальная отрисовка головы для читаемости
          if (i == 0) buffer[pos] = 4; 
          else buffer[pos] = 0; 
        }
      }
    }
  }
  
  // Рисуем яблоко
  int foodRow = foodY / 2;
  int foodCharRow = foodY % 2;
  if (foodRow < 2) {
    lcd.setCursor(foodX, foodRow);
    lcd.write(3); // Символ яблока
  }
  
  // Для тестирования
  for(int i=0; i<16; i++) {
     lcd.setCursor(i, 0);
     lcd.write(0); // Печать змейки блоками 
  }
}

void playTone(int frequency, int duration) {
  tone(buzzerPin, frequency, duration);
  delay(duration);
  noTone(buzzerPin);
}
