const int escPin1 = 9;
const int escPin2 = 11;
int throttle = 1350; // Ajusta entre 1000 y 2000
unsigned long previousMillis = 0;
const long interval = 3000; // 3 segundos
bool motor1Active = true; // Comienza con motor 1 activo

void setup() {
  Serial.begin(9600);
  pinMode(escPin1, OUTPUT);
  pinMode(escPin2, OUTPUT);

  // Armado del ESC 1
  Serial.println("Arming ESC 1...");
  for (int i = 0; i < 100; i++) {
    digitalWrite(escPin1, HIGH);
    delayMicroseconds(1000); // Mínimo throttle para armar
    digitalWrite(escPin1, LOW);
    delay(20);
  }
  
  // Armado del ESC 2
  Serial.println("Arming ESC 2...");
  for (int i = 0; i < 100; i++) {
    digitalWrite(escPin2, HIGH);
    delayMicroseconds(1000); // Mínimo throttle para armar
    digitalWrite(escPin2, LOW);
    delay(20);
  }
  
  Serial.println("Both ESCs Ready!");
  Serial.println("Starting alternating motor control...");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Verificar si han pasado 3 segundos
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    motor1Active = !motor1Active; // Cambiar estado del motor activo
    
    if (motor1Active) {
      Serial.println("Motor 1 ON - Motor 2 OFF");
    } else {
      Serial.println("Motor 2 ON - Motor 1 OFF");
    }
  }
  
  // Controlar motores según el estado actual
  if (motor1Active) {
    // Motor 1 girando, Motor 2 apagado
    digitalWrite(escPin1, HIGH);
    delayMicroseconds(throttle);
    digitalWrite(escPin1, LOW);
    
    digitalWrite(escPin2, HIGH);
    delayMicroseconds(1000); // Señal mínima para mantener ESC armado pero parado
    digitalWrite(escPin2, LOW);
  } else {
    // Motor 2 girando, Motor 1 apagado
    digitalWrite(escPin2, HIGH);
    delayMicroseconds(throttle);
    digitalWrite(escPin2, LOW);
    
    digitalWrite(escPin1, HIGH);
    delayMicroseconds(1000); // Señal mínima para mantener ESC armado pero parado
    digitalWrite(escPin1, LOW);
  }
  
  delay(20); // Refrescar cada 20 ms
}
