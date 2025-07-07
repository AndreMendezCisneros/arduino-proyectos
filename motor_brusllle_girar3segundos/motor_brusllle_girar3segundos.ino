const int escPin = 9;
int throttle = 1350;  // Ajusta entre 1000 y 2000

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
  // Motor encendido por 3 segundos
  Serial.println("Motor encendido");
  unsigned long startTime = millis();
  while (millis() - startTime < 3000) {
    digitalWrite(escPin, HIGH);
    delayMicroseconds(throttle);
    digitalWrite(escPin, LOW);
    delay(20); // Ciclo PWM cada 20ms
  }

  // Motor apagado por 3 segundos
  Serial.println("Motor apagado");
  startTime = millis();
  while (millis() - startTime < 3000) {
    digitalWrite(escPin, HIGH);
    delayMicroseconds(1000); // Throttle mínimo
    digitalWrite(escPin, LOW);
    delay(20); // Ciclo PWM cada 20ms
  }
}
