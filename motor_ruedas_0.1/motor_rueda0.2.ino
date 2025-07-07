const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;

// Secuencia para pasos completos (4 pasos)
int stepSequence[4][4] = {
  {1, 0, 1, 0},  // Paso 1
  {0, 1, 1, 0},  // Paso 2
  {0, 1, 0, 1},  // Paso 3
  {1, 0, 0, 1}   // Paso 4
};

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  // Girar a la derecha 100 pasos
  for (int i = 0; i < 100; i++) {
    stepMotor(i % 4);
    delay(10);
  }

  delay(1000); // Espera 1 segundo

  // Girar a la izquierda 100 pasos
  for (int i = 0; i < 100; i++) {
    stepMotor(3 - (i % 4)); // Reversa
    delay(10);
  }

  delay(1000); // Espera 1 segundo
}

void stepMotor(int step) {
  digitalWrite(IN1, stepSequence[step][0]);
  digitalWrite(IN2, stepSequence[step][1]);
  digitalWrite(IN3, stepSequence[step][2]);
  digitalWrite(IN4, stepSequence[step][3]);
}
