#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// --- CONFIGURACIÓN DE RED ---
const char* ssid = "completar_ssid";
const char* password = "completar_password";
// Nota: La IP asignada al ESP8266 debe ser accesible por el servidor web
// Si tu servidor web (index.js) corre en localhost, necesitarás usar la IP real
// del ESP8266 y configurar la variable MICRO_URL en el .env del servidor
// Ejemplo: MICRO_URL=http://192.168.1.100:5000 
// El puerto 5000 es el que usaremos, ya que es el que usa el Mock

ESP8266WebServer server(5000); 

// --- ESTADO DEL JUEGO (GLOBAL) ---
// Estado global que se mantiene actualizado por la EDU-CIAA y se envía al Servidor Web
struct GameState {
  int score = 0;
  int time_left = 60;
  String state = "idle"; // idle, running, finished
  int level = 1;
};
GameState currentState;

// Modo simulacion
unsigned long lastUpdate = 0;

// =======================================================
// 1. ENDPOINTS HTTP (ESP8266 -> Servidor del juego)
// =======================================================

/**
 * Endpoint: GET /status
 * El Servidor del juego llama aquí para obtener el estado actual.
 */
void handleStatus() {
  StaticJsonDocument<256> doc;
  
  doc["score"] = currentState.score;
  doc["time_left"] = currentState.time_left;
  doc["state"] = currentState.state;
  doc["level"] = currentState.level;
  doc["device"] = "tiro-al-blanco-esp";
  //doc["ts"] = new Date().toISOString(); // No es estrictamente necesario, pero simula el mock
  
  String output;
  serializeJson(doc, output);
  
  server.send(200, "application/json", output);
}

/**
 * Endpoint: POST /command
 * El Servidor del juego llama aquí para enviar comandos (START, RESET, etc.).
 */
void handleCommand() {
  
  // 1. Prepara la respuesta (con ok: true)
  StaticJsonDocument<512> doc_resp; // Usa doc_resp para la respuesta
  doc_resp["ok"] = true;

  if (server.hasArg("plain")) {
    StaticJsonDocument<256> doc_req; // Usa doc_req para la petición (request)
    DeserializationError error = deserializeJson(doc_req, server.arg("plain"));

    if (!error) {
      const char* command = doc_req["command"]; // Usa doc_req
      int level = doc_req["level"] | 1; 

      // *** Lógica de comandos (SIMULACIÓN) ***
      if (strcmp(command, "START") == 0) {
        currentState.state = "running";
        currentState.time_left = 60;
        currentState.score = 0;
        currentState.level = level;
        lastUpdate = millis(); 
      } else if (strcmp(command, "RESET") == 0) {
        currentState.state = "idle";
        currentState.time_left = 60;
        currentState.score = 0;
      } else if (strcmp(command, "SELECT_LEVEL") == 0) {
        currentState.level = level;
      }
      
      Serial.print("Comando web recibido: ");
      Serial.println(command);
    }
  }

  // 2. Construye el objeto 'state' para la respuesta JSON
  JsonObject state_resp = doc_resp.createNestedObject("state");
  state_resp["score"] = currentState.score;
  state_resp["time_left"] = currentState.time_left;
  state_resp["state"] = currentState.state;
  state_resp["level"] = currentState.level;
  state_resp["device"] = "tiro-al-blanco-esp";
  state_resp["ts"] = String(millis()); 

  String response_output; // Declara la variable para la salida
  serializeJson(doc_resp, response_output);
  
  // 3. Envía la respuesta JSON completa
  server.send(200, "application/json", response_output); // Usa response_output
}

// =======================================================
// 2. COMUNICACIÓN UART (EDU-CIAA -> ESP8266)
// =======================================================

/**
 * Revisa el puerto Serial para buscar actualizaciones de estado de la EDU-CIAA.
 */
void checkUart() {

  //Modo simulacion
  // *** SIMULACIÓN: Actualización de estado aleatorio ***
  if (currentState.state.equals("running") && (millis() - lastUpdate) >= 1000) {
    // 1. Decrementar tiempo
    if (currentState.time_left > 0) {
      currentState.time_left--;
    }
    
    // 2. Simular un acierto (cada 1 segundo con 30% de probabilidad)
    if (random(10) < 3) { // 30% de probabilidad de acierto
      currentState.score += (currentState.level == 3) ? random(10, 30) : random(5, 15);
    }
    
    // 3. Chequear fin de partida
    if (currentState.time_left <= 0) {
      currentState.state = "finished";
      Serial.println("Juego Finalizado por tiempo (Simulación).");
    }
    
    lastUpdate = millis(); // Reinicia el timer de simulación
  }
}

// =======================================================
// SETUP y LOOP
// =======================================================
void setup() {
  Serial.begin(115200); // UART a 115200 bps 
  
  randomSeed(analogRead(0)); // Inicializa la semilla para números aleatorios
  
  // Conexión Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Conectado.");
  Serial.print("ESP IP: ");
  Serial.println(WiFi.localIP());

  // Rutas del Servidor HTTP
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/command", HTTP_POST, handleCommand);
  
  server.begin();
  Serial.println("Servidor Mock (ESP8266) iniciado en el puerto 5000.");
}

void loop() {
  server.handleClient(); // Atiende las peticiones HTTP del servidor del juego
  checkUart();          // Revisa si la EDU-CIAA envió datos de estado
}
