#include <Stepper.h>  // Control del motor paso a paso

// --- Configuración del motor ---
const int STEPS_PER_REV = 100;
const int MOTOR_SPEED = 100;

const int MOTOR_PIN1 = 2;
const int MOTOR_PIN2 = 3;
const int MOTOR_PIN3 = 4;
const int MOTOR_PIN4 = 5;

Stepper motor(STEPS_PER_REV, MOTOR_PIN1, MOTOR_PIN2, MOTOR_PIN3, MOTOR_PIN4);

// --- Sensor ultrasónico ---
const int TRIG_PIN = 10;
const int ECHO_PIN = 11;
const float UMBRAL_DETECCION = 5.0;  // cm

bool objetoDetectadoAntes = false;

float medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracion = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout 30 ms
  if (duracion == 0) return -1;  // Sin eco válido

  float distancia = duracion * 0.0343 / 2;
  return distancia;
}

void setup() {
  motor.setSpeed(MOTOR_SPEED);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  float distancia = medirDistancia();

  Serial.print("Distancia: ");
  if (distancia < 0) {
    Serial.println("No válida");
    delay(200);
    return;
  }

  Serial.print(distancia);
  Serial.println(" cm");

  bool objetoDetectadoAhora = (distancia <= UMBRAL_DETECCION);

  if (objetoDetectadoAhora && !objetoDetectadoAntes) {
    Serial.println("🎯 Objeto detectado → motor gira 200 pasos horario");
    motor.step(-STEPS_PER_REV);  // Adelante
  } else if (!objetoDetectadoAhora && objetoDetectadoAntes) {
    Serial.println("🔄 Objeto retirado → motor gira 200 pasos antihorario");
    motor.step(STEPS_PER_REV);  // Atrás
  }

  objetoDetectadoAntes = objetoDetectadoAhora;

  delay(200);  // Pausa entre lecturas
}
