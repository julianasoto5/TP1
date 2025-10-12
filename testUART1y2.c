// --- EDU-CIAA - Prueba Bidireccional ---
#include "sapi.h"
#include <string.h>
#include <stdio.h>

#define ESP_UART_PORT   UART_232
#define UART_BAUD_RATE  115200

// =======================================================
// INICIALIZACIÓN
// =======================================================
void init() {
    boardConfig();
    uartConfig(ESP_UART_PORT, UART_BAUD_RATE); // RX/TX bidireccional
    uartConfig(UART_USB, UART_BAUD_RATE);      // Debug a la PC
}

// =======================================================
// CHEQUEO Y RESPUESTA
// =======================================================
void checkAndRespond() {
    uint8_t receivedByte;
    static char commandString[50];
    static int idx = 0;
    
    // 1. Intenta leer un byte del ESP (Llega de D4 del ESP)
    if (uartReadByte(ESP_UART_PORT, &receivedByte)) {
        
        // Acumula el byte
        if (idx < 49 && receivedByte != '\r') {
            commandString[idx++] = (char)receivedByte;
        }

        // Si detecta fin de línea, procesa
        if (receivedByte == '\n') {
            
            commandString[idx] = '\0'; // Termina la cadena
            idx = 0;                   // Reinicia el índice
            
            // 2. Compara el mensaje de prueba
            if (strncmp(commandString, "TEST:ESP_OK", 11) == 0) {
                
                // Mensaje recibido con éxito (Debug a PC)
                uartWriteString(UART_USB, "CIAA RCV: ");
                uartWriteString(UART_USB, commandString);
                uartWriteString(UART_USB, "\n\r");
                
                // 3. ENVÍA RESPUESTA (Sale por TXD_232, llega al pin RX del ESP)
                uartWriteString(ESP_UART_PORT, "CIAA_ACK\n");
                uartWriteString(UART_USB, "*** CIAA SENT: CIAA_ACK ***\n\r");
            }
            
            memset(commandString, 0, sizeof(commandString));
        }
    }
}

// =======================================================
// BUCLE PRINCIPAL
// =======================================================
int main(void) {
    init();
    delay(1000);
    uartWriteString(UART_USB, "\n\r--- CIAA: Test Bidireccional Activo ---\n\r");

    while (1) {
        checkAndRespond();
    }
    return 0;
}
