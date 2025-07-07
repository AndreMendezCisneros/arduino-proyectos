#include <SoftwareSerial.h>

SoftwareSerial BT(0, 1); // RX, TX

void setup() {
  Serial.begin(9600);
  BT.begin(9600);           // Velocidad del HM-10 BLE
  pinMode(9, OUTPUT);       // Pin del dispensador
  digitalWrite(9, LOW);     // Dispensador apagado al inicio
}

void loop() {
  if (BT.available()) {
    char dato = BT.read();

    if (dato == '1') {
      digitalWrite(9, HIGH);
      Serial.println("Dispensador ENCENDIDO");
    } else if (dato == '0') {
      digitalWrite(9, LOW);
      Serial.println("Dispensador APAGADO");
    } else {
      Serial.print("Dato no reconocido: ");
      Serial.println(dato);
    }
  }
}
