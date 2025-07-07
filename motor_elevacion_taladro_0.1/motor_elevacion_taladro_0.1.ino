#include <Stepper.h>

// -------------------------
// Código ORIGINAL de elevación (sin cambios)
// -------------------------
const int stepsPerRevolution = 100;
const int motorSpeed = 70;

const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;

const int pasosMaxArriba = 240;
const int pasosMaxAbajo = -210;

Stepper motor(stepsPerRevolution, IN1, IN2, IN3, IN4);
int pasosActuales = 0;

void setup() {
  motor.setSpeed(motorSpeed);
  Serial.begin(9600);
  Serial.println("Sistema iniciado. Posición actual: 0");

  // -------------------------
  // Configuración adicional del taladro
  // -------------------------
  pinMode(6, OUTPUT); // TALADRO_IN1
  pinMode(7, OUTPUT); // TALADRO_IN2
  pinMode(8, OUTPUT); // TALADRO_ENA
  pinMode(9, OUTPUT); // TALADRO_ENB (opcional si tienes 2 motores)
}

void loop() {
  // Movimiento hacia arriba en pasos de 10
  moverConTorque(pasosMaxArriba, 10);
  delay(1000);

  // Taladro gira en un sentido 5 segundos
  digitalWrite(6, HIGH);   // IN1
  digitalWrite(7, LOW);    // IN2
  digitalWrite(8, HIGH);   // ENA
  delay(5000);

  // Taladro se detiene
  digitalWrite(8, LOW);
  delay(500);

  // Taladro gira al revés 5 segundos
  digitalWrite(6, LOW);    // IN1
  digitalWrite(7, HIGH);   // IN2
  digitalWrite(8, HIGH);   // ENA
  delay(5000);

  // Taladro se detiene
  digitalWrite(8, LOW);
  delay(1000);

  // Movimiento hacia abajo en pasos de 10
  moverConTorque(pasosMaxAbajo, 10);
  delay(1000);
}

// -------------------------
// Función original sin cambios
// -------------------------
void moverConTorque(int destino, int pasosPorSegmento) {
  int direccion = (destino > pasosActuales) ? 1 : -1;

  while (pasosActuales != destino) {
    int pasosRestantes = abs(destino - pasosActuales);
    int pasosAMover = min(pasosPorSegmento, pasosRestantes);

    motor.step(pasosAMover * direccion);
    pasosActuales += pasosAMover * direccion;

    delay(50);

    Serial.print("Posición actual: ");
    Serial.println(pasosActuales);
  }
}
