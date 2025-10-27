/*#include "esp8266.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> // Para atoi

// Buffer para almacenar la IP (global, accesible via extern)
char esp_ip[IP_BUFFER_SIZE + 1] = "0.0.0.0";


// INICIALIZACIÓN

void esp8266_Init() {
    // La configuración de board ya se realiza en main.c
    uartConfig(ESP_UART_PORT, UART_BAUD_RATE); // UART para el ESP8266
    uartConfig(UART_USB, UART_BAUD_RATE);     // UART para el debug a la PC
    
    // Inicializar la IP como desconocida
    memset(esp_ip, 0, sizeof(esp_ip));
    strcpy(esp_ip, "0.0.0.0");

    // Imprimir mensaje de inicialización
    uartWriteString(UART_USB, "\n\r[DRV-ESP] Driver inicializado. Esperando IP...\n\r");
}

// =======================================================
// TRANSMISIÓN (TX) - Llamada por Tarea_Comunicacion
// =======================================================

void esp8266_SendStatus(uint16_t score, uint16_t time_left, char *state, uint8_t level) {
    char buffer[100];
    
    // El formato requerido por el protocolo serial
    sprintf(buffer, "STATUS:%d,%d,%s,%d\r\n", 
            score, 
            time_left, 
            state, 
            level);
            
    // Envío a ESP8266 y Debug a la PC
    uartWriteString(ESP_UART_PORT, buffer);
    
    // Debug a la PC (opcional, pero útil)
    uartWriteString(UART_USB, "[DRV-ESP:TX] "); 
    uartWriteString(UART_USB, buffer);
}

// =======================================================
// RECEPCIÓN (RX) - Llamada por Tarea_Comunicacion
// =======================================================

GameEvent_t esp8266_CheckCommands(void) {
    uint8_t receivedByte;
    // Deben ser static para mantener el estado entre llamadas RTOS (polling)
    static char commandString[50];
    static int idx = 0;
    
    GameEvent_t event = { .tipo = 0, .valor = 0 };

   // while (uartReadByte(ESP_UART_PORT, &receivedByte)) {
        
        // 1. Acumular el byte recibido
        if (idx < 49 && receivedByte != '\r') {
            commandString[idx++] = (char)receivedByte;
        }

        // 2. Si detecta fin de línea, procesar el mensaje
        if (receivedByte == '\n') {
            commandString[idx] = '\0'; // Terminar la cadena
            idx = 0;                   // Resetear el índice
            
            // Debug de lo que se recibió
            uartWriteString(UART_USB, "[DRV-ESP:RX] ");
            uartWriteString(UART_USB, commandString);

            
            // --- A. CHEQUEAR COMANDO DE JUEGO (COMMAND:...) ---
            if (strncmp(commandString, "COMMAND:", 8) == 0) {
                
                char *payload = commandString + 8; // Saltar "COMMAND:"
                char *command = strtok(payload, ":");
                char *levelStr = strtok(NULL, ":"); // Puede ser NULL

                int level = (levelStr != NULL) ? atoi(levelStr) : 0;
                
                if (strcmp(command, "START") == 0) {
                    event.tipo = START_GAME;
                    event.valor = (uint8_t)level;
                } else if (strcmp(command, "RESET") == 0) {
                    event.tipo = RESET;
                    event.valor = 0;
                } else if (strcmp(command, "SELECT_LEVEL") == 0) {
                    event.tipo = SELECT_LEVEL;
                    event.valor = (uint8_t)level;
                }
                
            }
            // --- B. CHEQUEAR DIRECCIÓN IP (IP:...) ---
            else if (strncmp(commandString, "IP:", 3) == 0) {
                char *ip_address = commandString + 3; // Saltar "IP:"
                // Guardar la IP recibida
                strncpy(esp_ip, ip_address, IP_BUFFER_SIZE);
                esp_ip[IP_BUFFER_SIZE] = '\0'; // Asegurar terminación
                uartWriteString(UART_USB, "[DRV-ESP] IP ALMACENADA.\n\r");
            }

            // 3. Limpiar buffer y salir si se encontró un evento de juego
            memset(commandString, 0, sizeof(commandString));
            if (event.tipo != 0) {
                return event; // Retorna el evento para que la Tarea_Comunicacion lo envíe a Juego.
            }
        }
    }
    
    // Si no se leyó nada o solo se recibió parte de una línea, retorna evento nulo.
    return event; 
}*/
/*
// --- ESTADO DEL JUEGO (Simulación) ---


GameState_t gameState;

// --- VARIABLES DE TIEMPO Y SIMULACIÓN ---
delay_t updateDelay;
int scoreIncrement = 10;
unsigned int seed = 0; 

// --- BUFFER PARA ALMACENAR LA IP ---
char esp_ip[IP_BUFFER_SIZE + 1] = "0.0.0.0";


// --- PROTOTIPOS ---


// =======================================================
// INICIALIZACIÓN
// =======================================================
void esp8266_Init() {
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


void checkCommandsFromESP() {
    uint8_t receivedByte;
    static char commandString[50];
    static int idx = 0;
     
    if (uartReadByte(ESP_UART_PORT, &receivedByte)) {
        uartWriteString(UART_USB, "if1\n\r"); 
        
        // Almacena el byte si hay espacio y no es retorno de carro
        if (idx < 49 && receivedByte != '\r') {
            commandString[idx++] = (char)receivedByte;
        }

        // Si detecta fin de línea, procesa el mensaje
        if (receivedByte == '\n') {
            uartWriteString(UART_USB, "if2\n\r"); 
            
            commandString[idx] = '\0'; // 1. Termina la cadena
            idx = 0;                   // 2. Resetea el índice para la próxima lectura
            
            uartWriteString(UART_USB, "CIAA RCV: ");
            uartWriteString(UART_USB, commandString);
            uartWriteString(UART_USB, "\n");
            
            if (strncmp(commandString, "TEST:", 5) == 0) {
                // Mensaje de prueba recibido
                uartWriteString(UART_USB, "\n\r*** TEST OK: Comunicacion UART verificada. ***\n");
                uartWriteString(UART_USB, "Esperando IP/Wi-Fi del ESP...\n\r");
            }
            // 1. CHEQUEAR COMANDO DE JUEGO (COMMAND:...)
            else if (strncmp(commandString, "COMMAND:", 8) == 0) {
                
                // --- Lógica de Comando (Sin Cambios) ---
                char *payload = commandString + 8; 
                char *command = strtok(payload, ":");
                char *levelStr = strtok(NULL, ":");
                int level = (levelStr != NULL) ? atoi(levelStr) : 1;

                if (strcmp(command, "START") == 0) {
                     // ... (Tu lógica de START) ...
                } 
                // ... (El resto de tu lógica de comandos) ...
            }
            
            // 2. CHEQUEAR DIRECCIÓN IP (IP:...)
            else if (strncmp(commandString, "IP:", 3) == 0) {
                
                char *ip_address = commandString + 3; // Saltar "IP:"
                
                // Asegurar que no se exceda el tamaño del buffer de IP
                strncpy(esp_ip, ip_address, IP_BUFFER_SIZE);
                esp_ip[IP_BUFFER_SIZE] = '\0'; // Asegurar terminación

                uartWriteString(UART_USB, "\n\r*** IP del ESP8266 recibida: ");
                uartWriteString(UART_USB, esp_ip);
                uartWriteString(UART_USB, " ***\n\r");
            }

            // 3. LIMPIAR BUFFER SOLO DESPUÉS DE PROCESAR
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


/*

// =======================================================
// BUCLE PRINCIPAL (main)
// =======================================================
int main(void) {
    // 1. Inicialización
    init();
    
    // 2. Sincronización de Terminal
    delay(5000); 

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
}*/