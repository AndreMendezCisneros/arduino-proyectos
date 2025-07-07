#define THERMISTORPIN A0
#define SERIESRESISTOR 10000    // Resistencia fija en ohmios (10k)
#define THERMISTORNOMINAL 10000 // Valor del termistor a 25°C
#define TEMPERATURENOMINAL 25   // Temperatura nominal (25°C)
#define BCOEFFICIENT 3950       // Coeficiente B del termistor

double readTemperature() {
  int adcValue = analogRead(THERMISTORPIN);

  double resistance = SERIESRESISTOR / ((1023.0 / adcValue) - 1.0);
  double steinhart;
  steinhart = resistance / THERMISTORNOMINAL;         // (R/Ro)
  steinhart = log(steinhart);                         // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                          // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);   // + (1/To)
  steinhart = 1.0 / steinhart;                        // Invertir
  steinhart -= 273.15;                                // Kelvin a Celsius

  return steinhart;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("Temperatura: ");
  Serial.print(readTemperature());
  Serial.println(" °C");
  delay(1000);
}
