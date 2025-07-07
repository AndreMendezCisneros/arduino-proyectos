#include <Stepper.h>

// Configuración del motor de pasos
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

// Pines del taladro
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
  // Subir con activación del taladro cada 20 pasos
  moverConTaladro(pasosMaxArriba, 70);

  delay(1000);

  // Bajar con activación del taladro cada 20 pasos
  moverConTaladro(pasosMaxAbajo, 70);

  delay(2000);
}

// Función para mover el motor (subir o bajar) con activación del taladro cada "pasosPorSegmento"
void moverConTaladro(int destino, int pasosPorSegmento) {
  int direccion = (destino > pasosActuales) ? 1 : -1;

  while (pasosActuales != destino) {
    int pasosRestantes = abs(destino - pasosActuales);
    int pasosAMover = min(pasosPorSegmento, pasosRestantes);

    motor.step(pasosAMover * direccion);
    pasosActuales += pasosAMover * direccion;

    Serial.print("Posición actual: ");
    Serial.println(pasosActuales);

    // Encender taladro 3 segundos
    activarTaladro(3000);

    delay(500); // pequeña pausa antes de continuar
  }
}

// Función para activar el taladro por un tiempo determinado
void activarTaladro(int duracionMs) {
  digitalWrite(TALADRO_IN1, HIGH);
  digitalWrite(TALADRO_IN2, LOW);
  digitalWrite(TALADRO_ENA, HIGH);

  Serial.println("Taladro encendido...");
  delay(duracionMs);

  digitalWrite(TALADRO_ENA, LOW);
  Serial.println("Taladro apagado.");
}
