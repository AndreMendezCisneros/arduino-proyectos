#include <NewPing.h>

const int escPin = 9;
const int trigPin = 11;
const int echoPin = 10;
const int maxDistance = 20; // 20 cm de distancia máxima para activación

int throttle = 1350;  // Ajusta entre 1000 y 2000
bool motorOn = false;

NewPing sonar(trigPin, echoPin, maxDistance);

void setup() {
  Serial.begin(9600);
  pinMode(escPin, OUTPUT);

  // Armado del ESC
  Serial.println("Arming ESC...");
  for (int i = 0; i < 100; i++) {
    digitalWrite(escPin, HIGH);
    delayMicroseconds(1000); // Mínimo throttle para armar
    digitalWrite(escPin, LOW);
    delay(20);
  }
  Serial.println("ESC Ready!");
}

void loop() {
  // Medir distancia con el sensor
  unsigned int distance = sonar.ping_cm();
  
  if (distance > 0 && distance <= maxDistance) {
    // Objeto detectado dentro del rango - encender motor
    if (!motorOn) {
      motorOn = true;
      Serial.println("Objeto detectado - Motor ENCENDIDO");
    }
  } else {
    // No hay objeto detectado - apagar motor
    if (motorOn) {
      motorOn = false;
      Serial.println("No hay objeto - Motor APAGADO");
      // Enviar señal de parada (1000 μs)
      digitalWrite(escPin, HIGH);
      delayMicroseconds(1000);
      digitalWrite(escPin, LOW);
      delay(20);
      return;
    }
  }

  if (motorOn) {
    // Mantener motor girando si está encendido
    digitalWrite(escPin, HIGH);
    delayMicroseconds(throttle);
    digitalWrite(escPin, LOW);
    delay(20); // Refrescar cada 20 ms
  }
}
