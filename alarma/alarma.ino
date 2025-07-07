void setup() {
  Serial.begin(9600);       // Iniciar comunicación serial
  pinMode(9, OUTPUT);       // Pin 9 como salida (PWM)
}

void loop() {
  byte brightness =0;
  if (Serial.available() > 0) {

    brightness=Serial.read();
    if (brightness >= '0' && brightness <= '9') {
      int brightness = 25 * (brightness - '0');  // Convertir de carácter a número (0-9) y escalar (0–225)

      analogWrite(9, brightness);         // Cambiar brillo del LED
    }
  }
}
