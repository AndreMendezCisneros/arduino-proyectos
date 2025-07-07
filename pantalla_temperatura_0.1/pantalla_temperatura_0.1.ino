#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Dirección I2C del LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int sensorPin = A0;

void setup() {
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura:");
}

void loop() {
  int valorAnalogico = analogRead(sensorPin);
  float voltaje = (valorAnalogico * 5.0) / 1023.0;
  float temperatura = voltaje * 100.0;

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperatura, 1); // 1 decimal
  lcd.print((char)223); // símbolo °
  lcd.print("C    "); // espacios para limpiar restos

  delay(1000);
}
