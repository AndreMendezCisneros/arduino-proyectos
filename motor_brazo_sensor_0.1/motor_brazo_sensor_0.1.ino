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

void setup() {
  motor.setSpeed(motorSpeed);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);  // Útil para pruebas si deseas ver la distancia
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
  duration = pulseIn(echoPin, HIGH);

  // Calcular distancia en cm
  distance = duration * 0.0343 / 2;

  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Si hay algo a 5 cm o menos, el motor gira sin parar
  if (distance > 0 && distance <= 5) {
    motor.step(stepsPerRevolution);  // Gira una vuelta
  } else {
    // Si no hay nada cerca, el motor no se mueve
    delay(100);
  }
}
