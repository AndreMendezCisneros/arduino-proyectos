#include <Wire.h>
const int ledPin = 13;

void setup() {
  Wire.begin(2); // Dirección del esclavo
  Wire.onReceive(recibirComando);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Esclavo iniciado...");
}

void loop() {
  // Nada aquí
}

void recibirComando(int bytes) {
  while (Wire.available()) {
    char cmd = Wire.read();
    Serial.print("Comando recibido: ");
    Serial.println(cmd);

    if (cmd == '1') {
      digitalWrite(ledPin, HIGH);
    } else if (cmd == '0') {
      digitalWrite(ledPin, LOW);
    }
  }
}
