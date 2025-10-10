#include "sapi.h"   
#include "string.h" 
#include <stdlib.h> 
#include <stdio.h>  

// --- CONSTANTES ---
// ¡IMPORTANTE! Asegúrate de que este sea el puerto que conecta la CIAA con el ESP8266
#define ESP_UART_PORT   UART_232 // Debería ser UART_232 o UART_485 para la comunicación real
#define UART_BAUD_RATE  115200 
#define IP_BUFFER_SIZE  16 // Suficiente para "xxx.xxx.xxx.xxx"

// --- ESTADO DEL JUEGO (Simulación) ---
typedef struct {
    int score;
    int time_left;
    char state[15]; 
    int level;
} GameState_t;

GameState_t gameState;

// --- VARIABLES DE TIEMPO Y SIMULACIÓN ---
delay_t updateDelay;
int scoreIncrement = 10;
unsigned int seed = 0; 

// --- BUFFER PARA ALMACENAR LA IP ---
char esp_ip[IP_BUFFER_SIZE + 1] = "0.0.0.0";


// --- PROTOTIPOS ---
void sendStatusToESP(void);
void checkCommandsFromESP(void);

// =======================================================
// INICIALIZACIÓN
// =======================================================
void init() {
    boardConfig(); 
    uartConfig(ESP_UART_PORT, UART_BAUD_RATE); // Puerto para el ESP8266
    uartConfig(UART_USB, UART_BAUD_RATE);      // Puerto para el debug a la PC
    delayConfig(&updateDelay, 1000); 
    
    seed = 123; 
    srand(seed); 

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
 */
void sendStatusToESP() {
    char buffer[100];
    
    sprintf(buffer, "STATUS:%d,%d,%s,%d\r\n", 
            gameState.score, 
            gameState.time_left, 
            gameState.state, 
            gameState.level);
            
    uartWriteString(ESP_UART_PORT, buffer);
    
    // Debug a la PC
    uartWriteString(UART_USB, "CIAA SNT: "); 
    uartWriteString(UART_USB, buffer);
}

/**
 * Chequea si el ESP8266 envió un comando (COMMAND:...) o su IP (IP:...)
 */
void checkCommandsFromESP() {
    uint8_t receivedByte;
    static char commandString[50];
    static int idx = 0;
    
    if (uartReadByte(ESP_UART_PORT, &receivedByte)) {
        
        if (idx < 49 && receivedByte != '\r') {
            commandString[idx++] = (char)receivedByte;
        }

        if (receivedByte == '\n') {
            
            commandString[idx] = '\0'; 
            idx = 0; 
            
           if (strncmp(commandString, "TEST:", 5) == 0) {
                // Mensaje de prueba recibido
                uartWriteString(UART_USB, "\n\r*** TEST OK: Comunicacion UART verificada. ***\n");
                uartWriteString(UART_USB, "Esperando IP/Wi-Fi del ESP...\n\r");
            }
            // 1. CHEQUEAR COMANDO DE JUEGO (COMMAND:...)
            else if (strncmp(commandString, "COMMAND:", 8) == 0) {
                // ... (Lógica de COMMAND sin cambios) ...
                char *payload = commandString + 8; 
                char *command = strtok(payload, ":");
                char *levelStr = strtok(NULL, ":");
                int level = (levelStr != NULL) ? atoi(levelStr) : 1;

                uartWriteString(UART_USB, "CIAA RCV: ");
                uartWriteString(UART_USB, commandString);
                uartWriteString(UART_USB, "\n");

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
            
            // 2. CHEQUEAR DIRECCIÓN IP (IP:...)
            } else if (strncmp(commandString, "IP:", 3) == 0) {
                
                char *ip_address = commandString + 3; // Saltar "IP:"
                
                // Asegurar que no se exceda el tamaño del buffer de IP
                strncpy(esp_ip, ip_address, IP_BUFFER_SIZE);
                esp_ip[IP_BUFFER_SIZE] = '\0'; // Asegurar terminación

                uartWriteString(UART_USB, "\n\r*** IP del ESP8266 recibida: ");
                uartWriteString(UART_USB, esp_ip);
                uartWriteString(UART_USB, " ***\n\r");
            }

            memset(commandString, 0, sizeof(commandString));
        }
    }
}

// =======================================================
// LÓGICA DE SIMULACIÓN (dentro de la CIAA)
// =======================================================

void runGameSimulation() {
    if (strcmp(gameState.state, "running") == 0) {
        // ... (Lógica de simulación sin cambios) ...
        if (gameState.time_left > 0) {
            gameState.time_left--;
        } else {
            strcpy(gameState.state, "finished");
            uartWriteString(UART_USB, "CIAA: Juego FINALIZADO por tiempo.\n");
        }
        
        if ((rand() % 10) < 3) { 
            gameState.score += scoreIncrement * gameState.level;
        }
    }
}

// =======================================================
// BUCLE PRINCIPAL (main)
// =======================================================
int main(void) {
    // 1. Inicialización
    init();
    
    // 2. Sincronización de Terminal
    delay(100); 

    // 3. Mensajes de Bienvenida e Indicación de Espera
    uartWriteString(UART_USB, "\n\r--- EDU-CIAA: Modo Prueba UART/ESP ---\n");
    uartWriteString(UART_USB, "Conectado al ESP8266 en el puerto ");
    // ... (Bloque de debug del puerto) ...
    uartWriteString(UART_USB, "Esperando IP del ESP... (Puede tardar varios segundos)\n\r");
    
    // 4. Bucle de Espera Activa
    while (strcmp(esp_ip, "0.0.0.0") == 0) {
        // Solo chequeamos comandos (incluyendo la IP) sin enviar nada.
        checkCommandsFromESP();
        delay(10); 
    }

    // 5. Mensaje de Continuación
    uartWriteString(UART_USB, "IP RECIBIDA. Iniciando bucle de juego...\n\r");
    
    // 6. Bucle Principal (Aplicación/Juego)
    while (1) {
        checkCommandsFromESP();
        
        if (delayRead(&updateDelay)) {
            runGameSimulation();
            sendStatusToESP();
        }
    }
    return 0;
}
