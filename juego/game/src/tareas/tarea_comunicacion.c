#include "userTasks.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "tarea_juego.h"
#include "esp8266.h" 

QueueHandle_t xColaRx;

extern QueueHandle_t xColaJuego;
extern QueueHandle_t xColaComunicacion;
extern TaskHandle_t xComunicationHandler;
extern TaskHandle_t xJuegoHandler;

// El estado sombra se queda en la tarea, ya que es
// parte de la l�gica de la tarea (recordar estado).
static uint16_t local_score = 0;
static uint16_t  local_time = MAX_TIEMPO_PARTIDA/100;
static uint8_t  local_level = NIVEL_FACIL;
static GameState_t local_state = STATE_IDLE;



void checkCommandsFromESP(void* n){
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   char c;
   while (uartRxReady(UART_232)) {
      c = uartRxRead(UART_232);
      xQueueSendFromISR(xColaRx, &c, &xHigherPriorityTaskWoken);
   }
   
   vTaskNotifyGiveFromISR(xComunicationHandler, &xHigherPriorityTaskWoken);
   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void tarea_comunicacion(void *pvParameters) {
    GameEvent_t evento;
    xColaRx = xQueueCreate(50, sizeof(char));
    ESP_Driver_Init(UART_232, 115200);
    uartCallbackSet(UART_232, UART_RECEIVE, checkCommandsFromESP, NULL);
   // Habilito todas las interrupciones de ESP_UART_PORT
    uartInterrupt(UART_232, true);
    for(;;) {
      gpioWrite(LED1, OFF);
      gpioWrite(LED2, ON);
      gpioWrite(LED3, OFF);
      gpioWrite(LEDR, OFF);
      gpioWrite(LEDB, OFF);
      ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        // 1. REVISAR COLA DE JUEGO (CIAA -> ESP)
        if (xQueueReceive(xColaComunicacion, &evento, 0) == pdPASS) {
            // Actualizar el estado sombra
            switch (evento.tipo) {
                case START_GAME: 
                    local_state = STATE_PLAYING;
                    local_score = 0;
                    local_time = MAX_TIEMPO_PARTIDA/100;
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
                    local_time = MAX_TIEMPO_PARTIDA/100;
                    break;
            }
            
            // Enviar estado actualizado al driver
           // printf("[COMUNICACION] Local time: %d\r\n",local_time);
            ESP_Driver_SendStatus(local_score, local_time, local_state, local_level);
            //xTaskNotifyGive(xJuegoHandler);
        }

        // 2. REVISAR UART (ESP -> CIAA)
        // El driver nos dice si tiene un comando listo
        char c;
        
        if(xQueueReceive(xColaRx, &c, 0) == pdPASS) {//se recibio algo
            if(ESP_Driver_GetCommand(c, &evento)){// Si hay comando, se lo pasamos a tarea_juego
               xQueueSend(xColaJuego, &evento, 0);
               //printf("[COMUNICACION] Comando enviado: %d\r\n", evento.tipo);
               //xTaskNotifyGive(xJuegoHandler);
               //gpioWrite(LED3, ON);
            }
        }
        
        
        // Ceder el CPU - Lo comento porque si cede CPU no lee bien comandos 
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
