#include <Stepper.h>

// -------------------------
// Configuración del motor paso a paso
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

// -------------------------
// Configuración del motor DC (taladro)
// -------------------------
const int TALADRO_IN1 = 6;
const int TALADRO_IN2 = 7;
const int TALADRO_ENA = 8;

void setup() {
  motor.setSpeed(motorSpeed);
  Serial.begin(9600);
  Serial.println("Sistema iniciado. Posición actual: 0");

  pinMode(TALADRO_IN1, OUTPUT);
  pinMode(TALADRO_IN2, OUTPUT);
  pinMode(TALADRO_ENA, OUTPUT);
}

void loop() {
  // Movimiento hacia arriba
  moverConTorque(pasosMaxArriba, 10);
  delay(1000);

  // Movimiento hacia abajo
  moverConTorque(pasosMaxAbajo, 10);
  delay(1000);
}

// -------------------------
// Función de movimiento con taladro activo
// -------------------------
void moverConTorque(int destino, int pasosPorSegmento) {
  int direccion = (destino > pasosActuales) ? 1 : -1;

  // Taladro gira según dirección
  if (direccion == 1) {
    digitalWrite(TALADRO_IN1, HIGH);
    digitalWrite(TALADRO_IN2, LOW);
  } else {
    digitalWrite(TALADRO_IN1, LOW);
    digitalWrite(TALADRO_IN2, HIGH);
  }
  digitalWrite(TALADRO_ENA, HIGH);  // Enciende taladro

  while (pasosActuales != destino) {
    int pasosRestantes = abs(destino - pasosActuales);
    int pasosAMover = min(pasosPorSegmento, pasosRestantes);

    motor.step(pasosAMover * direccion);
    pasosActuales += pasosAMover * direccion;

    delay(50);

    Serial.print("Posición actual: ");
    Serial.println(pasosActuales);
  }

  digitalWrite(TALADRO_ENA, LOW);  // Apaga taladro al terminar
}
