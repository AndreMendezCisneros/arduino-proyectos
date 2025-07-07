// Pines conectados al puente H
const int IN1 = 8;   // Dirección 1
const int IN2 = 9;   // Dirección 2
const int ENA = 10;  // Enable (puede ser PWM)

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
}

void loop() {
  // Avanza 5 segundos
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(ENA, HIGH);  // Activar motor
  delay(5000);

  // Se detiene 1 segundo
  digitalWrite(ENA, LOW);   // Apagar motor
  delay(1000);

  // Retrocede 5 segundos
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(ENA, HIGH);  // Activar motor
  delay(5000);

  // Se detiene 1 segundo
  digitalWrite(ENA, LOW);   // Apagar motor
  delay(1000);
}
