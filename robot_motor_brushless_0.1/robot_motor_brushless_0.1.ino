  const int escPin =9;
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
    // Mantener motor girando
    digitalWrite(escPin, HIGH);
    delayMicroseconds(throttle); // Puede ser 1300–1600 según tu ESC
    digitalWrite(escPin, LOW);
    delay(20); // Refrescar cada 20 ms
  }
