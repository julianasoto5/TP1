#include <Stepper.h>
const int stepsPerRevolution = 2048; // 28BYJ-48
// Pines de Arduino Mega conectados al ULN2003: D2, D3, D4, D5
Stepper myStepper(stepsPerRevolution, 2, 4, 3, 5);
void setup() {
  Serial.begin(115200);
  myStepper.setSpeed(10); // Velocidad en RPM
  Serial.println("Motor listo");
}
void loop() {
  Serial.println("Gira una vuelta en sentido horario...");
  myStepper.step(stepsPerRevolution);   // Una vuelta completa
  delay(1000);
  Serial.println("Gira una vuelta en sentido antihorario...");
  myStepper.step(-stepsPerRevolution);  // Vuelta contraria
  delay(1000);
}
