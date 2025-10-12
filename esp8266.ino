#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// --- CONFIGURACIÓN DE RED ---
// ¡REEMPLAZA TUS CREDENCIALES AQUÍ!
const char* ssid = "Fibertel WiFi516 2.4GHz"; 
const char* password = "416727349"; 

ESP8266WebServer server(5000); 

// --- ESTADO DEL JUEGO (GLOBAL) ---
struct GameState {
  int score = 0;
  int time_left = 60;
  String state = "idle"; // idle, running, finished
  int level = 1;
};
GameState currentState;


// =======================================================
// 1. ENDPOINTS HTTP (Servidor Web <-> ESP8266)
// =======================================================

/**
 * Endpoint: GET /status
 * Responde con el estado actual del juego.
 */
void handleStatus() {
  StaticJsonDocument<256> doc;
  
  doc["score"] = currentState.score;
  doc["time_left"] = currentState.time_left;
  doc["state"] = currentState.state;
  doc["level"] = currentState.level;
  doc["device"] = "tiro-al-blanco-esp";
  
  String output;
  serializeJson(doc, output);
  
  server.send(200, "application/json", output);
}

/**
 * Endpoint: POST /command
 * Recibe comandos del servidor web y los envía a la EDU-CIAA por UART.
 * Responde con la estructura JSON correcta que espera el servidor web.
 */
void handleCommand() {
  
  // 1. Prepara la respuesta (con ok: true)
  StaticJsonDocument<512> doc_resp; 
  doc_resp["ok"] = true;
  const char* command = "UNKNOWN";

  if (server.hasArg("plain")) {
    StaticJsonDocument<256> doc_req; 
    DeserializationError error = deserializeJson(doc_req, server.arg("plain"));

    if (!error) {
      command = doc_req["command"] | "UNKNOWN";
      int level = doc_req["level"] | 1; 

      // *** LÓGICA DE COMUNICACIÓN UART (ENVÍO a EDU-CIAA) ***
      String commandString = "COMMAND:";
      commandString += command;
      commandString += ":";
      commandString += String(level);
      
      Serial.println(commandString); // Envía el comando al Serial (EDU-CIAA)
      
      Serial.print("Comando web enviado por UART: ");
      Serial.println(commandString);
      // Fin Lógica UART
    }
  }

  // 2. Construye el objeto 'state' para la respuesta JSON (siguiendo el formato del mock)
  JsonObject state_resp = doc_resp.createNestedObject("state");
  state_resp["score"] = currentState.score;
  state_resp["time_left"] = currentState.time_left;
  state_resp["state"] = currentState.state;
  state_resp["level"] = currentState.level;
  state_resp["device"] = "tiro-al-blanco-esp";
  state_resp["ts"] = String(millis()); 

  String response_output; 
  serializeJson(doc_resp, response_output);
  
  // 3. Envía la respuesta JSON completa
  server.send(200, "application/json", response_output); 
}

// =======================================================
// 2. COMUNICACIÓN UART (EDU-CIAA -> ESP8266)
// =======================================================

/**
 * Revisa el puerto Serial para buscar actualizaciones de estado de la EDU-CIAA.
 */
void checkUart() {
  // *** LÓGICA DE COMUNICACIÓN UART (RECEPCIÓN de EDU-CIAA) ***
  while (Serial.available()) {
    String data = Serial.readStringUntil('\n'); // Lee el mensaje completo hasta el salto de línea
    data.trim(); 

    if (data.startsWith("STATUS:")) {
      // Formato esperado: STATUS:<score>,<time_left>,<state>,<level>
      String payload = data.substring(7); // Quita "STATUS:"
      
      int pos1 = payload.indexOf(',');
      int pos2 = payload.indexOf(',', pos1 + 1);
      int pos3 = payload.indexOf(',', pos2 + 1);

      if (pos1 > 0 && pos2 > 0 && pos3 > 0) {
        // Actualizar las variables globales del estado
        currentState.score     = payload.substring(0, pos1).toInt();
        currentState.time_left = payload.substring(pos1 + 1, pos2).toInt();
        currentState.state     = payload.substring(pos2 + 1, pos3);
        currentState.level     = payload.substring(pos3 + 1).toInt();

        currentState.state.trim();
        
        Serial.println("DEBUG: Estado actualizado por EDU-CIAA.");
      }
    }
  }
}

// =======================================================
// SETUP y LOOP
// =======================================================

void setup() {
    // 1. Inicializa Serial para DEBUG (PC) y RECEPCIÓN (pin RX) - SEGURO
    Serial.begin(115200); 
    
    // NO PONEMOS Serial.println() AQUÍ, lo ponemos después del delay.
    
    // Conexión Wi-Fi (Proceso largo y estable)
    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi Conectado.");

    // AHORA es el momento SEGURO para inicializar Serial1 (D4) y enviar datos.
    // 2. Inicializa Serial1 para TRANSMISIÓN (pin D4 a CIAA)
    Serial1.begin(115200); 

    // Mensaje de prueba (DEBE salir por Serial1)
    Serial1.println("TEST:UART1_OK"); 
    Serial.println("NodeMCU: UARTs inicializados. TEST:UART1_OK enviado.");

    delay(2000);
    
    // Envía la IP a la CIAA en el formato (USANDO Serial1)
    String ipMessage = "IP:";
    ipMessage += WiFi.localIP().toString();
    Serial1.println(ipMessage); 
    Serial.println(ipMessage); // Debug a PC

    // Rutas del Servidor HTTP
    server.on("/status", HTTP_GET, handleStatus);
    server.on("/command", HTTP_POST, handleCommand);
    
    server.begin();
    Serial.println("Servidor Proxy (ESP8266) iniciado en el puerto 5000.");
}

void loop() {
  server.handleClient(); // Atiende las peticiones HTTP
  checkUart();          // Revisa el Serial para recibir datos de la EDU-CIAA
}
