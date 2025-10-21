#include "esp8266.h"


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