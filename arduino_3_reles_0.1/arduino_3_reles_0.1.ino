// Definir pines de relé
const int rele1 = 9;
const int rele2 = 10;
const int rele3 = 11;

void setup() {
  // Configurar pines como salida
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);

  // Asegurar que los relés estén apagados al inicio
  digitalWrite(rele1, LOW);
  digitalWrite(rele2, LOW);
  digitalWrite(rele3, LOW);
}

void loop() {
  // Encender relé 1 por 5 segundos
  digitalWrite(rele1, HIGH);
  delay(5000);
  digitalWrite(rele1, LOW);

  // Encender relé 2 por 5 segundos
  digitalWrite(rele2, HIGH);
  delay(5000);
  digitalWrite(rele2, LOW);

  // Encender relé 3 por 5 segundos
  digitalWrite(rele3, HIGH);
  delay(5000);
  digitalWrite(rele3, LOW);
}
