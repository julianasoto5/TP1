#include "userTasks.h" 
#include "sapi.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
#include "tarea_juego.h"
#include <stdlib.h> // Para atoi

#define UART_ESP            UART_232 
#define UART_ESP_BAUDRATE   115200
#define RX_BUFFER_SIZE      64

extern QueueHandle_t xColaJuego;
extern QueueHandle_t xColaComunicacion;


static void ParseComandoESP(char* buffer);
static const char* EstadoJuegoToString(GameState_t estado);
static void EnviarEstadoESP();

// La tarea de comunicación mantiene su propia copia del estado para poder enviar el paquete "STATUS" completo.
static uint16_t local_score = 0;
static uint8_t  local_time = MAX_TIEMPO_PARTIDA;
static uint8_t  local_level = NIVEL_FACIL; 
static GameState_t local_state = STATE_IDLE; // Estado inicial


/* Parsea un comando recibido del ESP8266 y lo envía a tarea_juego. Recibe como parametro un String (terminado en NULL) con el comando. */
static void ParseComandoESP(char* buffer) {
    GameEvent_t cmd_evento;
    bool_t send_cmd = true;
    char* token;
    
    // Protocolo ESP: COMMAND:NAME:VALUE\n
    
    if (strncmp(buffer, "COMMAND:START:", 14) == 0) {
        cmd_evento.tipo = START_GAME;
        cmd_evento.valor = (uint8_t)atoi(buffer + 14); 
        printf("[COM] Comando RX: START (Nivel %d)\r\n", cmd_evento.valor);

    } else if (strncmp(buffer, "COMMAND:RESET:", 14) == 0) {
        cmd_evento.tipo = GAME_RESET;
        cmd_evento.valor = 0;
        printf("[COM] Comando RX: RESET\r\n");
       
    } else if (strncmp(buffer, "COMMAND:GAME_OVER:", 18) == 0) {
        cmd_evento.tipo = GAME_OVER;
        cmd_evento.valor = 0;
        printf("[COM] Comando RX: GAME_OVER\r\n");
      

    } else if (strncmp(buffer, "COMMAND:SELECT_LEVEL:", 21) == 0) {
        cmd_evento.tipo = SELECT_LEVEL;
        cmd_evento.valor = (uint8_t)atoi(buffer + 21);
        printf("[COM] Comando RX: SELECT_LEVEL (Nivel %d)\r\n", cmd_evento.valor);
       
    } else {
        send_cmd = false;
        printf("[COM] Comando RX desconocido: %s\r\n", buffer);
    }

    if (send_cmd) {
       
       if (cmd_evento.tipo == START_GAME) {
          local_state = STATE_PLAYING;
          local_score = 0;
          local_time = MAX_TIEMPO_PARTIDA;
          local_level = cmd_evento.valor; 
          EnviarEstadoESP(); 
       } else if (cmd_evento.tipo == GAME_OVER) {
            local_state = STATE_GAME_OVER;
            local_time = 0; // Asumir tiempo restante cero
            EnviarEstadoESP(); 
        } else if (cmd_evento.tipo == GAME_RESET) {
            local_state = STATE_IDLE;
            local_score = 0;
            local_time = MAX_TIEMPO_PARTIDA;
            EnviarEstadoESP();
        }
         xQueueSend(xColaJuego, &cmd_evento, 0);
    }
}

/*Convierte el enum de estado del juego a un string para el ESP.*/
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

/*Envía el estado actual del juego al ESP8266.*/
static void EnviarEstadoESP() {
    char tx_buffer[64];
    // Protocolo: STATUS:SCORE,TIME,STATE,LEVEL\r\n
    sprintf(tx_buffer, "STATUS:%lu,%u,%s,%u\r\n",
           (unsigned long)local_score,
            local_time,
            EstadoJuegoToString(local_state),
            local_level);
    uartWriteString(UART_ESP, tx_buffer);
}


void Comunicacion_Init(void) {
    // Configurar la UART para comunicarse con el ESP8266
    uartConfig(UART_ESP, UART_ESP_BAUDRATE); 
    printf("[COM] Tarea de Comunicacion inicializada. UART_232 a %d baudios.\r\n", UART_ESP_BAUDRATE);
}

void tarea_comunicacion(void *pvParameters) {
    GameEvent_t evento_rx_juego; // Evento recibido de tarea_juego
    
    // Buffer para recepción de UART
    static char rx_buffer[RX_BUFFER_SIZE];
    static uint8_t rx_index = 0;
    uint8_t byte_recibido;
    Comunicacion_Init();
    while (1) {
        // 1. REVISAR COLA DE JUEGO (CIAA -> ESP)
        if (xQueueReceive(xColaComunicacion, &evento_rx_juego, 0) == pdPASS) {
            bool_t estado_cambiado = true;
            switch (evento_rx_juego.tipo) {
                case DISPARO:
                    local_score = evento_rx_juego.valor; // Recibe puntaje TOTAL
                    break;
                case TICK_1S:
                    local_time = evento_rx_juego.valor; // Recibe tiempo restante
                    break;
                default:
                    estado_cambiado = false;
                    break;
            }
            // Si hubo un cambio, envia stado actualizado al ESP
            if (estado_cambiado) {
                EnviarEstadoESP();
            }
        }

            
        // 2. REVISAR UART (ESP -> CIAA)
        // Leemos todos los bytes disponibles en el buffer de la UART (no bloqueante)
        while (uartReadByte(UART_ESP, &byte_recibido)) {
            if (byte_recibido == '\n') { // Fin de comando
                rx_buffer[rx_index] = '\0'; // Terminar el string
                
                if (rx_index > 0) { // Si no es una línea vacía
                    ParseComandoESP(rx_buffer);
                }
                rx_index = 0; // Resetear buffer

            } else if (byte_recibido != '\r') { // Ignorar carriage return
                if (rx_index < (RX_BUFFER_SIZE - 1)) {
                    rx_buffer[rx_index++] = byte_recibido;
                } else {
                    // Error: Buffer overflow. Resetear.
                    rx_index = 0;
                }
            }
        }
    }
}