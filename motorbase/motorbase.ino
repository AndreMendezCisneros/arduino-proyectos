#include <Stepper.h>  //Motor base

const int stepsPerRevolution = 200;
const int motorSpeed = 100;

const int pinMotorA = 2;
const int pinMotorB = 3;
const int pinMotorC = 4;
const int pinMotorD = 5;

Stepper motor(stepsPerRevolution, pinMotorA, pinMotorB, pinMotorC, pinMotorD);

void setup() {
  motor.setSpeed(motorSpeed);
  Serial.begin(9600);
}

void loop() {
  Serial.println("Giro horario");
  motor.step(200);     // Una vuelta completa en sentido horario
  delay(1000);

  Serial.println("Giro antihorario");
  motor.step(-200);    // Una vuelta completa en sentido antihorario
  delay(1000);
}
