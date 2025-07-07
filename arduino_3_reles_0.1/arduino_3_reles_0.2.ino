// Pines de los relés
const int rele1 = 9;
const int rele2 = 10;
const int rele3 = 11;

void setup() {
  // Configurar los pines como salidas
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);

  // Asegurar que todos los relés estén apagados al inicio
  digitalWrite(rele1, LOW);
  digitalWrite(rele2, LOW);
  digitalWrite(rele3, LOW);
}

void loop() {
  // Apagar todos antes de encender el siguiente
  digitalWrite(rele1, HIGH);
  digitalWrite(rele2, LOW);
  digitalWrite(rele3, LOW);
  delay(10000);

  digitalWrite(rele1, LOW);
  digitalWrite(rele2, HIGH);
  digitalWrite(rele3, LOW);
  delay(10000);

  digitalWrite(rele1, LOW);
  digitalWrite(rele2, LOW);
  digitalWrite(rele3, HIGH);
  delay(10000);

  // Se repite infinitamente
}
