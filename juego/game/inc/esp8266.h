#ifndef ESP8266_H_
#define ESP8266_H_

#include "sapi.h"        // Incluye las librerías de la CIAA (UART, etc.)
#include "tarea_juego.h" // Incluye GameEvent_t

// --- CONSTANTES DEL DRIVER ---
//#define ESP_UART_PORT   UART_2    // Puerto conectado al ESP8266
#define UART_BAUD_RATE  115200
#define IP_BUFFER_SIZE  16        // Espacio para almacenar la IP

// --- PROTOTIPOS PÚBLICOS ---

void esp8266_Init(void);
void esp8266_SendStatus(uint16_t score, uint16_t time_left, char *state, uint8_t level);
GameEvent_t esp8266_CheckCommands(void);

// --- VARIABLES GLOBALES DEL DRIVER ---
extern char esp_ip[IP_BUFFER_SIZE + 1];

#endif /* ESP8266_H_ */