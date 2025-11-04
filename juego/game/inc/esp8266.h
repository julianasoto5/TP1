#ifndef ESP8266_H_
#define ESP8266_H_

#include "sapi.h"        // Incluye las librerías de la CIAA (UART, etc.)
#include "tarea_juego.h" // Incluye GameEvent_t

// --- CONSTANTES DEL DRIVER ---
//#define ESP_UART_PORT   UART_2    // Puerto conectado al ESP8266
#define UART_BAUD_RATE  115200
#define IP_BUFFER_SIZE  16        // Espacio para almacenar la IP

// --- PROTOTIPOS PÚBLICOS ---

// Inicializa el driver en una UART espec�fica
void ESP_Driver_Init(uartMap_t uart, uint32_t baudrate);

// Env�a el paquete de estado al ESP
void ESP_Driver_SendStatus(uint16_t score, uint8_t time, GameState_t state, uint8_t level);

// Revisa si hay un comando completo y válido en el buffer de la UART.
// Si lo hay, lo carga en 'event' y retorna TRUE.
bool_t ESP_Driver_GetCommand(GameEvent_t* event);

// --- VARIABLES GLOBALES DEL DRIVER ---
extern char esp_ip[IP_BUFFER_SIZE + 1];

#endif /* ESP8266_H_ */
