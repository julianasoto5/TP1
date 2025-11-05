#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <stdlib.h>


int score = 0;
int time_left = 0;
String state = "idle"; 
int level = 1;
unsigned long lastUpdate = 0;


void handleStatus() {
  StaticJsonDocument<200> doc;
  //doc["score"] = score;
  doc["score"] = String(score);
  doc["time_left"] = time_left;
  doc["state"] = state;
  doc["level"] = level;
  doc["device"] = "mesa1";
  doc["ts"] = millis();

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

/*
void handleCommand() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    server.send(400, "text/plain", "Bad JSON");
    return;
  }

  String command = doc["command"] | "";
  int newLevel = doc["level"] | level;

  if (command == "START") {
    state = "running";
    time_left = 60;
    score = 0;
    level = newLevel;
  } else if (command == "RESET") {
    state = "idle";
    time_left = 60;
    score = 0;
  } else if (command == "SELECT_LEVEL") {
    level = newLevel;
  }
  Serial.println(command);
  server.send(200, "application/json", "{\"ok\":true}");
}
*/

void handleCommand() {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    server.send(400, "text/plain", "Bad JSON");
    return;
  }
  String command = doc["command"] | "";
  int newLevel = doc["level"] | level;


  if (command == "START") {    
    Serial.print("COMMAND:START:");
    Serial.println(newLevel); 
    Serial1.print("COMMAND:START:");
    Serial1.println(newLevel);

  } else if (command == "RESET") {
    Serial.println("COMMAND:RESET:"); 
    Serial1.println("COMMAND:RESET:"); 

  } else if (command == "SELECT_LEVEL") { 
    Serial.print("COMMAND:SELECT_LEVEL:");
    Serial.println(newLevel); 
    Serial1.print("COMMAND:SELECT_LEVEL:");
    Serial1.println(newLevel); 


  } else if (command == "GAME_OVER") {
     Serial1.print("COMMAND:GAME_OVER:"); 
  } 
  else {
    server.send(400, "text/plain", "Unknown command");
    return;
  }
  server.send(200, "application/json", "{\"ok\":true}");
}

void setup() {
  // UART0 para el Monitor Serial de la PC (Debug)
  Serial.begin(115200);
  Serial1.begin(115200);
  //Serial1.swap();
 // Serial.println("\nMonitor de Debug (UART0) iniciado.");

  // UART1 para la comunicación con la CIAA (TX en D4/GPIO2)
  //Serial1.begin(115200);
  //Serial.println("Comunicacion CIAA (UART1) iniciada en D4 (GPIO2).");
  WiFi.softAP("ESP8266-Juego", "clave123"); // SSID y contraseña
  IPAddress ip = WiFi.softAPIP();
  Serial.println("Access Point IP: " + ip.toString());
  

 /* while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado. IP: " + WiFi.localIP().toString());*/

  server.on("/status", HTTP_GET, handleStatus);
  server.on("/command", HTTP_POST, handleCommand);
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
  
  // Lógica para recibir el estado del juego de la CIAA
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    
    // El protocolo es STATUS:SCORE,TIME,STATE,LEVEL\r\n
    if (input.startsWith("STATUS:")) {
      String payload = input.substring(7); // Quitar "STATUS:"
      
      int sep1 = payload.indexOf(',');
      int sep2 = payload.indexOf(',', sep1 + 1);
      int sep3 = payload.indexOf(',', sep2 + 1);

      if (sep3 > 0) {
        // 1. Obtener la subcadena del score
          String scoreStr = payload.substring(0, sep1);

        // 2. Convertir la subcadena String a char*
          char score_buffer[scoreStr.length() + 1];
          scoreStr.toCharArray(score_buffer, sizeof(score_buffer));

        // 3. Usar strtoul para parsear como unsigned long
          score = strtoul(score_buffer, NULL, 10);
          //score = payload.substring(0, sep1).toInt();
          time_left = payload.substring(sep1 + 1, sep2).toInt();
          state = payload.substring(sep2 + 1, sep3);
          level = payload.substring(sep3 + 1).toInt();

      }
    }
  }
}
