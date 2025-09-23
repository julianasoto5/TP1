void setup() {
  Serial.begin(115200);
}
void loop() {
  int V = analogRead(A0);
  Serial.println(V);
  delay(1000);
}
