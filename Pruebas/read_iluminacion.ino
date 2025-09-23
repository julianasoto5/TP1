const long A = 1000; // Resistencia en oscuridad (KΩ)
const int B = 15;    // Resistencia a la luz (KΩ)
const int Rc = 10;   // Resistencia fija en el divisor (KΩ)
void setup() {
  Serial.begin(115200);
}
void loop() {
  int V = analogRead(A0);
  int ilum = ((long)V*A*10)/((long)B*Rc*(1024-V));
  Serial.print("Valor ADC: ");
  Serial.print(V);
  Serial.print(" -> Lux aproximados: ");
  Serial.println(ilum);
  delay(1000);
}
