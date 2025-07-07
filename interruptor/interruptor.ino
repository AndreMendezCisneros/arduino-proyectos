void setup() {
  Serial.begin(9600);      // Iniciar comunicación serial
  pinMode(9, OUTPUT);      // Configurar el pin 9 como salida
  digitalWrite(9, LOW);    // Asegurarse de que el dispensador empiece apagado
}

void loop() {
  if (Serial.available()) {
    char dato = Serial.read();   // Leer el dato ingresado por el usuario

    if (dato == '1') {
      digitalWrite(9, HIGH);     // Encender dispensador
      Serial.println("Dispensador ENCENDIDO");
    } else if (dato == '0') {
      digitalWrite(9, LOW);      // Apagar dispensador
      Serial.println("Dispensador APAGADO");
    } else {
      Serial.println("Comando inválido. Ingrese 1 para encender o 0 para apagar.");
    }
  }
}
