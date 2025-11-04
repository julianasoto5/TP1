#include "userTasks.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "tarea_juego.h"
#include "esp8266.h" 

extern QueueHandle_t xColaJuego;
extern QueueHandle_t xColaComunicacion;

// El estado sombra se queda en la tarea, ya que es
// parte de la l�gica de la tarea (recordar estado).
static uint16_t local_score = 0;
static uint8_t  local_time = MAX_TIEMPO_PARTIDA;
static uint8_t  local_level = NIVEL_FACIL;
static GameState_t local_state = STATE_IDLE;

void tarea_comunicacion(void *pvParameters) {
    GameEvent_t evento;
    
    ESP_Driver_Init(UART_232, 115200);

    while (1) {
        
        // 1. REVISAR COLA DE JUEGO (CIAA -> ESP)
        if (xQueueReceive(xColaComunicacion, &evento, 0) == pdPASS) {
            
            // Actualizar el estado sombra
            switch (evento.tipo) {
                case START_GAME: 
                    local_state = STATE_PLAYING;
                    local_score = 0;
                    local_time = MAX_TIEMPO_PARTIDA;
                    local_level = evento.valor;
                    break;
                case DISPARO:
                    local_score = evento.valor;
                    break;
                case TICK_1S:
                    local_time = evento.valor;
                    break;
                case FIN_PARTIDA: 
                    local_state = STATE_GAME_OVER;
                    local_score = evento.valor;
                    local_time = 0;
                    break;
                 case GAME_RESET:
                    local_state = STATE_IDLE;
                    local_score = 0;
                    local_time = MAX_TIEMPO_PARTIDA;
                    break;
            }
            
            // Enviar estado actualizado al driver
            ESP_Driver_SendStatus(local_score, local_time, local_state, local_level);
        }

        // 2. REVISAR UART (ESP -> CIAA)
        // El driver nos dice si tiene un comando listo
        if (ESP_Driver_GetCommand(&evento)) {
            // Si hay comando, se lo pasamos a tarea_juego
            xQueueSend(xColaJuego, &evento, 0);
        }
        
        // Ceder el CPU - Lo comento porque si cede CPU no lee bien comandos 
       // vTaskDelay(pdMS_TO_TICKS(10));
    }
}
