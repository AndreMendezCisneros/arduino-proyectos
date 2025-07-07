#include <Stepper.h>

// Configuración del motor (HS200 2221 0070 AF.04)
const int stepsPerRevolution = 100;  // 1.8° por paso
const int motorSpeed = 70;           // Velocidad reducida para más torque

// Conexiones al L298N (solo las 4 entradas necesarias)
const int IN1 = 2;  // Conectar a IN1 del L298N
const int IN2 = 3;  // Conectar a IN2 del L298N
const int IN3 = 4;  // Conectar a IN3 del L298N
const int IN4 = 5;  // Conectar a IN4 del L298N

// Límites virtuales de movimiento
const int pasosMaxArriba = 240;
const int pasosMaxAbajo = -210;

// Inicialización del motor
Stepper motor(stepsPerRevolution, IN1, IN2, IN3, IN4);
int pasosActuales = 0;  // Para llevar registro de la posición

void setup() {
  motor.setSpeed(motorSpeed);  // Configura velocidad
  
  // No necesitamos configurar ENA/ENB porque:
  // 1. Tu módulo no los tiene expuestos
  // 2. Probablemente están puenteados internamente a HIGH
  
  Serial.begin(9600);
  Serial.println("Sistema iniciado. Posición actual: 0");
}

void loop() {
  // Movimiento hacia arriba en pasos de 10
  moverConTorque(pasosMaxArriba, 10);
  delay(1000);
  
  // Movimiento hacia abajo en pasos de 10
  moverConTorque(pasosMaxAbajo, 10);
  delay(1000);
}

// Función mejorada para movimiento con control de torque
void moverConTorque(int destino, int pasosPorSegmento) {
  // Determina dirección (1 para arriba, -1 para abajo)
  int direccion = (destino > pasosActuales) ? 1 : -1;
  
  while (pasosActuales != destino) {
    // Calcula cuántos pasos mover en este segmento
    int pasosRestantes = abs(destino - pasosActuales);
    int pasosAMover = min(pasosPorSegmento, pasosRestantes);
    
    // Ejecuta el movimiento
    motor.step(pasosAMover * direccion);
    
    // Actualiza la posición
    pasosActuales += pasosAMover * direccion;
    
    // Pequeña pausa para mejor torque
    delay(50);
    
    // Feedback por serial
    Serial.print("Posición actual: ");
    Serial.println(pasosActuales);
  }
}
