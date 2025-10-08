#include "sapi.h"   // Biblioteca de la CIAA: boardConfig, uartConfig, uartReadByte, uartWriteString, delay, etc.
#include "string.h" // Para manejo de strings (strcmp, strncmp, etc.)
#include <stdlib.h> // Para atoi (conversión de ASCII a entero)
#include <stdio.h>  // Para sprintf (construcción de strings formateados)

#define ESP_UART_PORT   UART_232 
#define UART_BAUD_RATE  115200 // Debe coincidir con el ESP8266

// --- ESTADO DEL JUEGO (Simulación) ---
typedef struct {
    int score;
    int time_left;
    char state[15]; // "idle", "running", "finished"
    int level;
} GameState_t;

GameState_t gameState;

// --- VARIABLES DE TIEMPO Y SIMULACIÓN ---
delay_t updateDelay;
int scoreIncrement = 10;
unsigned int seed = 0; // Para inicializar rand()

// --- PROTOTIPOS ---
void sendStatusToESP(void);
void checkCommandsFromESP(void);

// =======================================================
// INICIALIZACIÓN
// =======================================================
void boardInit() {
    boardConfig(); // Inicializa la placa
    uartConfig(ESP_UART_PORT, UART_BAUD_RATE); // Inicializa la comunicación Serial
    delayConfig(&updateDelay, 1000); // Configura un timer de 1000 ms (1 segundo)
    
    // Inicializar el generador de números pseudo-aleatorios (simulando seed en el ejemplo del ESP)
    // Usamos un valor de ADC (si está disponible) o el tiempo. Aquí usamos un valor fijo para simplificar.
    // En un hardware real, se usaría un pin analógico o un timer.
    seed = 123; 
    srand(seed); 

    // Inicializar el estado
    gameState.score = 0;
    gameState.time_left = 60;
    strcpy(gameState.state, "idle");
    gameState.level = 1;
}

// =======================================================
// FUNCIONES DE COMUNICACIÓN
// =======================================================

/**
 * Envía el estado del juego al ESP8266.
 * Formato: STATUS:<score>,<time_left>,<state>,<level>\n
 */
void sendStatusToESP() {
    char buffer[100];
    
    // sprintf es mucho más limpio que usar 'itoa' o aritmética manual para concatenar números.
    // Usamos \r\n al final, aunque el ESP solo busca '\n', el \r ayuda a la limpieza del Serial.
    sprintf(buffer, "STATUS:%d,%d,%s,%d\r\n", 
            gameState.score, 
            gameState.time_left, 
            gameState.state, 
            gameState.level);
            
    uartWriteString(ESP_UART_PORT, buffer);
    
    // Envía un mensaje de debug al mismo puerto (si es UART_USB, lo verás en la terminal)
    uartWriteString(UART_USB, "CIAA SNT: "); 
    uartWriteString(UART_USB, buffer);
}

/**
 * Chequea si el ESP8266 envió un comando (START, RESET)
 * Formato esperado: COMMAND:<nombre>:<level>\n
 */
void checkCommandsFromESP() {
    uint8_t receivedByte;
    static char commandString[50];
    static int idx = 0;
    
    // uartReadByte retorna true si hay un byte disponible para leer
    if (uartReadByte(ESP_UART_PORT, &receivedByte)) {
        
        // Almacenar el byte, evitando desbordamiento y caracteres de control (como \r)
        if (idx < 49 && receivedByte != '\r') {
            commandString[idx++] = (char)receivedByte;
        }

        // Si recibimos el fin de línea (\n), procesamos el comando
        if (receivedByte == '\n') {
            
            commandString[idx] = '\0'; // Asegurar terminador de string
            idx = 0; // Reiniciar el índice
            
            // Si el comando inicia con COMMAND:
            if (strncmp(commandString, "COMMAND:", 8) == 0) {
                
                // --- Extraer el payload: COMMAND:<nombre>:<level> ---
                char *payload = commandString + 8; // Saltar "COMMAND:"
                char *command = strtok(payload, ":");
                char *levelStr = strtok(NULL, ":");
                int level = (levelStr != NULL) ? atoi(levelStr) : 1;

                uartWriteString(UART_USB, "CIAA RCV: ");
                uartWriteString(UART_USB, commandString);
                uartWriteString(UART_USB, "\n");

                // --- LÓGICA DE COMANDOS ---
                if (strcmp(command, "START") == 0) {
                    strcpy(gameState.state, "running");
                    gameState.score = 0;
                    gameState.time_left = 60;
                    gameState.level = level;
                    uartWriteString(UART_USB, "CIAA: Juego INICIADO.\n");
                    sendStatusToESP(); 
                } else if (strcmp(command, "RESET") == 0) {
                    strcpy(gameState.state, "idle");
                    gameState.score = 0;
                    gameState.time_left = 60;
                    uartWriteString(UART_USB, "CIAA: Juego RESETEADO.\n");
                    sendStatusToESP(); 
                } else if (strcmp(command, "SELECT_LEVEL") == 0) {
                    gameState.level = level;
                    uartWriteString(UART_USB, "CIAA: Nivel cambiado.\n");
                }
            }
            // Limpiar el buffer si no se procesó
            memset(commandString, 0, sizeof(commandString));
        }
    }
}

// =======================================================
// LÓGICA DE SIMULACIÓN (dentro de la CIAA)
// =======================================================

void runGameSimulation() {
    // Si el juego está corriendo, actualizamos el estado
    if (strcmp(gameState.state, "running") == 0) {
        
        // 1. Decrementar tiempo y Chequear fin de partida
        if (gameState.time_left > 0) {
            gameState.time_left--;
        } else {
            strcpy(gameState.state, "finished");
            uartWriteString(UART_USB, "CIAA: Juego FINALIZADO por tiempo.\n");
        }
        
        // 2. Simular un acierto (cada 1 segundo con 30% de probabilidad)
        if ((rand() % 10) < 3) { 
            gameState.score += scoreIncrement * gameState.level;
        }
    }
}

// =======================================================
// BUCLE PRINCIPAL (main)
// =======================================================
int main(void) {
    boardInit();
    
    // Mensaje inicial de bienvenida
    uartWriteString(UART_USB, "\n\r--- EDU-CIAA: Modo Prueba UART/ESP ---\n");
    uartWriteString(UART_USB, "Esperando comando START desde el ESP8266...\n");
    
    while (1) {
        // 1. Verificar comandos entrantes desde el ESP8266 (Canal de bajada)
        checkCommandsFromESP();
        
        // 2. Ejecutar la lógica de simulación cada 1000 ms (1 segundo)
        if (delayRead(&updateDelay)) {
            runGameSimulation();
            
            // 3. Enviar el estado actualizado al ESP8266 (Canal de subida)
            sendStatusToESP();
        }
    }
    return 0;
}
