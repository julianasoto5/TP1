#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "Barcala 2.4G";
const char* password = "barcala2024";

ESP8266WebServer server(80);

// Estado del juego
int score = 0;
int time_left = 60;
String state = "idle"; // idle, running, finished
int level = 1;
unsigned long lastUpdate = 0;

void handleStatus() {
  StaticJsonDocument<200> doc;
  doc["score"] = score;
  doc["time_left"] = time_left;
  doc["state"] = state;
  doc["level"] = level;
  doc["device"] = "mesa1";
  doc["ts"] = millis();

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

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
  Serial.print(command);
  server.send(200, "application/json", "{\"ok\":true}");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado. IP: " + WiFi.localIP().toString());

  server.on("/status", HTTP_GET, handleStatus);
  server.on("/command", HTTP_POST, handleCommand);
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}
/*void loop() {
  server.handleClient();

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    int sep1 = input.indexOf(',');
    if (sep1 > 0) {
      score = input.substring(0, sep1).toInt();
      time_left = input.substring(sep1 + 1).toInt();
      state = "running"; // o lo que corresponda
    }
  }
}
*/
void loop() {
  server.handleClient();

  // Simulación de avance del juego
  if (state == "running" && millis() - lastUpdate > 1000) {
    lastUpdate = millis();
    time_left--;
    score += 10;
    if (time_left <= 0) {
      time_left = 0;
      state = "finished";
    }
  }
}
