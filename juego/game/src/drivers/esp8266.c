#include "esp8266.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define RX_BUFFER_SIZE 64

static uartMap_t driver_uart;
static char rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_index = 0;

// ----- Funciones Privadas -----

static bool_t ParseComando(char* buffer, GameEvent_t* event) {
    bool_t send_cmd = true;
    
    if (strncmp(buffer, "COMMAND:START:", 14) == 0) {
        event->tipo = START_GAME;
        event->valor = (uint8_t)atoi(buffer + 14);
    } else if (strncmp(buffer, "COMMAND:RESET:", 14) == 0) {
        event->tipo = GAME_RESET;
        event->valor = 0;
    } else if (strncmp(buffer, "COMMAND:GAME_OVER:", 18) == 0) {
         event->tipo = GAME_OVER; 
         event->valor = 0;
    } else if (strncmp(buffer, "COMMAND:SELECT_LEVEL:", 21) == 0) {
        event->tipo = SELECT_LEVEL;
        event->valor = (uint8_t)atoi(buffer + 21);
    } else {
        printf("[DRIVER] Comando RX desconocido: %s\r\n", buffer);
        send_cmd = false;
    }
    return send_cmd;
}

static const char* EstadoJuegoToString(GameState_t estado) {
     switch (estado) {

        case STATE_IDLE:
        case STATE_READY:
            return "idle";
        case STATE_PLAYING:
            return "running";
        case STATE_GAME_OVER:
            return "finished";
    }
    return "idle";
}


}

// ----- Funciones Públicas -----

void ESP_Driver_Init(uartMap_t uart, uint32_t baudrate) {
    driver_uart = uart;
    uartConfig(driver_uart, baudrate);
    printf("[DRIVER] ESP Driver inicializado. UART_232 a %d baudios.\r\n", baudrate);
}


void ESP_Driver_SendStatus(uint16_t score, uint8_t time, GameState_t state, uint8_t level) {
    char tx_buffer[64];
    sprintf(tx_buffer, "STATUS:%lu,%u,%s,%u\r\n",
           (unsigned long)score, time, EstadoJuegoToString(state), level);
    uartWriteString(driver_uart, tx_buffer);
}

bool_t ESP_Driver_GetCommand(GameEvent_t* event) {
    uint8_t byte_recibido;
    bool_t command_received = false;

    while (uartReadByte(driver_uart, &byte_recibido)) {
        if (byte_recibido == '\n') {
            rx_buffer[rx_index] = '\0';
            if (rx_index > 0) {
                // Llama al parser y actualiza el evento si es válido
                if (ParseComando(rx_buffer, event)) {
                    command_received = true;
                }
            }
            rx_index = 0;
            
            // Si encontramos un comando, salimos del while
            // para procesarlo en la tarea
            if(command_received) {
                break;
            }

        } else if (byte_recibido != '\r') {
            if (rx_index < (RX_BUFFER_SIZE - 1)) {
                rx_buffer[rx_index++] = byte_recibido;
            } else {
                rx_index = 0; // Overflow
            }
        }
    }
    
    return command_received;
}
