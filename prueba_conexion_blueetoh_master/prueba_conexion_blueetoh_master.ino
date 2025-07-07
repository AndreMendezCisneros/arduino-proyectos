#include <Wire.h>
#include <SoftwareSerial.h>
//master
// Corrección: RX = 2, TX = 3 desde el punto de vista del Arduino
SoftwareSerial BT(3, 2); 

void setup() {
  Wire.begin();        // Maestro I2C
  BT.begin(9600);      // Bluetooth
  Serial.begin(9600);  // Monitor Serial
  Serial.println("Bluetooth activo, esperando comandos...");
}

void loop() {
  if (BT.available()) {
    char comando = BT.read();
    Serial.print("Recibido por Bluetooth: ");
    Serial.println(comando);

    // Enviar comando al esclavo
    Wire.beginTransmission(2); // Dirección del esclavo
    Wire.write(comando);       // '1' o '0'
    Wire.endTransmission();

    Serial.println("Comando enviado al esclavo.");
  }
}
