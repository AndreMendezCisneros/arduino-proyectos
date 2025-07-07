#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  sensors.begin();
  sensors.setWaitForConversion(true); // Necesario en modo parásito
}

void loop() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);

  if (temp == DEVICE_DISCONNECTED) { // Cambiado aquí
    Serial.println("Sensor no detectado o fallo en la lectura.");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println(" °C");
  }

  delay(1000);
}
