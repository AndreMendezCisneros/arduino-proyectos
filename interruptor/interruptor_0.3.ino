#include <SoftwareSerial.h>

SoftwareSerial BT(2, 3); // RX, TX

const int pinDispensador = 9;

void setup() {
  Serial.begin(9600);
  BT.begin(9600);          
  pinMode(pinDispensador, OUTPUT);
  digitalWrite(pinDispensador, LOW);
}

void loop() {
  if (BT.available()) {
    char dato = BT.read();

    if (dato == '1') {
      digitalWrite(pinDispensador, HIGH);
      Serial.println("Dispensador ENCENDIDO");
    } else if (dato == '0') {
      digitalWrite(pinDispensador, LOW);
      Serial.println("Dispensador APAGADO");
    } else {
      Serial.print("Dato no reconocido: ");
      Serial.println(dato);
    }
  }
}
