#include <Stepper.h> //motor base

// Configuración del motor
const int stepsPerRevolution = 200;  // Pasos por revolución
const int motorSpeed = 100;          // Velocidad en RPM (reducida para más torque)

// Pines de control (conectados a IN1-IN4 del L298N)
const int pinMotorA = 2;
const int pinMotorB = 3;
const int pinMotorC = 4;
const int pinMotorD = 5;

// Tamaño del segmento para movimiento con torque
const int pasosPorSegmento = 10;  // Movimiento en pequeños pasos

Stepper motor(stepsPerRevolution, pinMotorA, pinMotorB, pinMotorC, pinMotorD);

void setup() {
  motor.setSpeed(motorSpeed);
  Serial.begin(9600);
  Serial.println("Motor inicializado - Modo alto torque");
}

void loop() {
  // Giro horario con control de torque
  Serial.println("Giro horario (alta torque)");
  moverConTorque(600);  // 200 pasos = 1 vuelta completa
  
  delay(1000);  // Pausa entre movimientos
  
  // Giro antihorario con control de torque
  Serial.println("Giro antihorario (alta torque)");
  moverConTorque(-600); // -200 pasos = 1 vuelta inversa
  
  delay(1000);  // Pausa entre movimientos
}

// Función mejorada para movimiento con torque
void moverConTorque(int pasosTotales) {
  int pasosRestantes = abs(pasosTotales);
  int direccion = (pasosTotales > 0) ? 1 : -1;
  
  while (pasosRestantes > 0) {
    int pasosAMover = min(pasosPorSegmento, pasosRestantes);
    
    motor.step(pasosAMover * direccion);
    pasosRestantes -= pasosAMover;
    
    // Pequeña pausa para permitir que el motor desarrolle torque
    delay(30);
    
    // Feedback opcional (puedes comentarlo si no lo necesitas)
    Serial.print("Pasos restantes: ");
    Serial.println(pasosRestantes);
  }
}
