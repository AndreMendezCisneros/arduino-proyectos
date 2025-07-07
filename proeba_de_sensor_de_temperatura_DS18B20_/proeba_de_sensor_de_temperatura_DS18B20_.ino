#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just DS18B20)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Example");

  // Start up the library
  sensors.begin();
}

void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  // It takes 750ms to get the temperature, so don't be surprised if it doesn't print instantly

  // You can also get temperature from a specific sensor by its address.
  // For simplicity, we'll just get the first one found on the bus.
  // If you have multiple DS18B20 sensors, you'll need to identify them by their unique address.

  Serial.print("Temperature for the first sensor: ");
  Serial.print(sensors.getTempCByIndex(0)); // Get temperature in Celsius
  Serial.print(" Celsius / ");
  Serial.print(sensors.getTempFByIndex(0)); // Get temperature in Fahrenheit
  Serial.println(" Fahrenheit");

  delay(1000); // Wait 1 second before taking another reading
}
