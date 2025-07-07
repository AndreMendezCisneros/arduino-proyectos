#include <Stepper.h>  // Motor base

// Configuración del motor
const int stepsPerRevolution = 200;
const int motorSpeed = 100;

const int pinMotorA = 2;
const int pinMotorB = 3;
const int pinMotorC = 4;
const int pinMotorD = 5;

Stepper motor(stepsPerRevolution, pinMotorA, pinMotorB, pinMotorC, pinMotorD);

// Pines del sensor ultrasónico
const int trigPin = 10;
const int echoPin = 11;

// Estado anterior de detección
bool objetoDetectadoAntes = false;

void setup() {
  motor.setSpeed(motorSpeed);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);  // Para monitoreo
}

void loop() {
  long duration;
  float distance;

  // Enviar pulso ultrasónico
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Leer duración del eco
  duration = pulseIn(echoPin, HIGH, 30000);  // Timeout de 30ms
  distance = duration * 0.0343 / 2;          // Convertir a cm

  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Detecta objeto a 5 cm o menos
  bool objetoDetectadoAhora = (distance > 0 && distance <= 5);

  if (objetoDetectadoAhora && !objetoDetectadoAntes) {
    // Se detectó un objeto (antes no había)
    motor.step(stepsPerRevolution);  // Gira 200 pasos horario
  } else if (!objetoDetectadoAhora && objetoDetectadoAntes) {
    // El objeto ya no está (antes sí había)
    motor.step(-stepsPerRevolution);  // Gira 200 pasos antihorario
  }

  objetoDetectadoAntes = objetoDetectadoAhora;  // Actualiza estado

  delay(200);  // Pequeña pausa para evitar lecturas muy rápidas
}
