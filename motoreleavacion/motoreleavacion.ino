#include <Stepper.h> //Motor elevacion

// Configuración del motor (HS200 2221 0070 AF.04)
const int stepsPerRevolution = 100;  // 1.8° por paso
const int motorSpeed = 70;           // Reduce la velocidad para más torque

// Pines conectados al motor (usa todos los pines del L298N)
const int pinMotorA = 2;  // IN1 en L298N
const int pinMotorB = 3;  // IN2 en L298N
const int pinMotorC = 4;  // IN3 en L298N
const int pinMotorD = 5;  // IN4 en L298N

// Límites virtuales
const int pasosMaxArriba = 240;
const int pasosMaxAbajo = -210;

Stepper motor(stepsPerRevolution, pinMotorA, pinMotorB, pinMotorC, pinMotorD);
int pasosActuales = 0;

void setup() {
  motor.setSpeed(motorSpeed);
  // Asegúrate de que los pines de habilitación del L298N estén HIGH
  pinMode(8, OUTPUT);  // ENA (habilitación puente A)
  pinMode(9, OUTPUT);  // ENB (habilitación puente B)
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  
  Serial.begin(9600);
  Serial.println("Motor listo. Posición inicial: 0");
}

void loop() {
  // Usa pasos más pequeños para evitar perder pasos
  moverConTorque(pasosMaxArriba, 10);  // Sube en pasos de 10
  delay(1000);
  moverConTorque(pasosMaxAbajo, 10);   // Baja en pasos de 10
  delay(1000);
}

// Función mejorada con control de torque
void moverConTorque(int destino, int pasosPorSegmento) {
  int direccion = (destino > pasosActuales) ? 1 : -1;
  
  while (pasosActuales != destino) {
    int pasosRestantes = abs(destino - pasosActuales);
    int pasosAMover = min(pasosPorSegmento, pasosRestantes);
    
    motor.step(pasosAMover * direccion);
    pasosActuales += pasosAMover * direccion;
    
    // Pequeña pausa para permitir que el motor desarrolle torque
    delay(50);
    
    Serial.print("Posición actual: ");
    Serial.println(pasosActuales);
  }
}
