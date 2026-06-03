#include <Stepper.h>

// Количество шагов на один оборот вашего мотора (например, 200 для NEMA17, 32 для 28BYJ-48)
const int stepsPerRevolution = 2048; 

// Инициализация библиотеки. Укажите пины, к которым подключен драйвер
// Для A4988/DRV8825: используем (шаг, направление)
// Для ULN2003: (шагов, IN1, IN3, IN2, IN4)
Stepper myStepper(stepsPerRevolution, 2, 3, 4, 5);

void setup() {
  // Устанавливаем скорость вращения (в об/мин)
  myStepper.setSpeed(10);
}

void loop() {
  // Вращаем мотор на 1 оборот (полный круг) по часовой стрелке
  myStepper.step(stepsPerRevolution);
  delay(1000); // Пауза 1 секунда

  // Вращаем мотор на 1 оборот против часовой стрелки
  myStepper.step(-stepsPerRevolution);
  delay(1000); // Пауза 1 секунда
}
