// --- ESP8266 (NodeMCU) - Prueba Bidireccional ---

String esp_message = "TEST:ESP_OK"; 
int counter = 0;

void setup() {
  // 1. RECEPCIÓN (CIAA -> ESP) usa Serial (RX pin)
  Serial.begin(115200); 
  
  // 2. TRANSMISIÓN (ESP -> CIAA) usa Serial1 (D4 pin)
  Serial1.begin(115200); 
  
  delay(1000); 
  Serial.println("NodeMCU: UARTs inicializados. Enviando TEST a CIAA...");
}

void loop() {
  // A. TRANSMISIÓN (Sale por D4, llega a RXD_232 de la CIAA)
  if (counter % 50 == 0) { // Envía un mensaje cada 5 segundos (aprox)
    String tx_msg = esp_message + String(counter / 50);
    Serial1.println(tx_msg); 
    Serial.print("ESP SENT: ");
    Serial.println(tx_msg);
  }

  // B. RECEPCIÓN (Llega por el pin RX, se lee con Serial)
  while (Serial.available()) {
    String received_data = Serial.readStringUntil('\n');
    received_data.trim();
    
    if (received_data.startsWith("CIAA_ACK")) {
      Serial.println("*** EXITO RECIBIDO: CIAA ACKNOWLEDGE! ***");
    } else {
      Serial.print("ESP RCV: Datos inesperados: ");
      Serial.println(received_data);
    }
  }

  delay(100);
  counter++;
}
