#include "tarea_juego.h"
#include "userTasks.h"
#include <stdio.h>
#include "semphr.h"

#include "displayTM1637.h"
extern QueueHandle_t xColaJuego;
extern QueueHandle_t xColaFeedback;
extern QueueHandle_t xColaComunicacion;
extern QueueHandle_t xColaMovimiento;
extern TaskHandle_t xComunicationHandler, xMatrizLDRHandler, xFeedbackHandler;
TimerHandle_t timerJuego;
SemaphoreHandle_t xServoSemaphore1 = NULL, xServoSemaphore2 = NULL;
static GameContext_t contexto;

/* ---- Configuración general ---- */

static void TimerCallbackJuego(TimerHandle_t xTimer);


void Juego_Init(void)
{
    timerJuego = xTimerCreate("TimerJuego", pdMS_TO_TICKS(10), pdTRUE, 0, TimerCallbackJuego); //deberia ser cada 10ms porque el display va a tener esa resolucion
    xServoSemaphore1 = xSemaphoreCreateBinary();
    xServoSemaphore2 = xSemaphoreCreateBinary();
    contexto.estado = STATE_IDLE;
    contexto.nivel = NIVEL_DIFICIL;
    contexto.puntaje = 0;
    contexto.tiempo_restante = MAX_TIEMPO_PARTIDA;
}

/* Env�o de eventos desde tareas  */
void Juego_EnviarEvento(GameEventType_t tipo, uint8_t val)
{
    GameEvent_t evento = { .tipo = tipo, .valor = val };
    xQueueSend(xColaJuego, &evento, 0);
}

/* Callback del timer de 10ms */
volatile uint8_t counter = 0;
           
GameEvent_t timerEvent ={. tipo = TICK_10MS, .valor = 0};
static void TimerCallbackJuego(TimerHandle_t xTimer)
{
   timerEvent.valor=--contexto.tiempo_restante;
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   xQueueSendFromISR(xColaJuego, &timerEvent, &xHigherPriorityTaskWoken);
   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


/* Máquina de estados  */
void tarea_juego(void *pvParameters)
{
   GameEvent_t evento;
   Juego_Init();
   
   //Servo_Init();
    for(;;) {
      gpioWrite(LED1, ON);
      gpioWrite(LED2, OFF);
      gpioWrite(LED3, OFF);
      gpioWrite(LEDR, OFF);
      gpioWrite(LEDB, OFF);
            switch (contexto.estado) {

            case STATE_IDLE:
                contexto.puntaje = 0;
                contexto.tiempo_restante = MAX_TIEMPO_PARTIDA;
                printf("Esperando evento en xColaJuego - STATE_IDLE\r\n");
                
                vTaskPrioritySet(xComunicationHandler, tskIDLE_PRIORITY+5);
                vTaskPrioritySet(xMatrizLDRHandler, tskIDLE_PRIORITY);
                vTaskPrioritySet(xServoHandler, tskIDLE_PRIORITY);
                
                if (xQueueReceive(xColaJuego, &evento, portMAX_DELAY)) {
                    if (evento.tipo == SELECT_LEVEL) {
                        contexto.nivel = evento.valor;
                    }
                    else if (evento.tipo == START_GAME) {
                        contexto.estado = STATE_READY;
                        contexto.nivel = evento.valor;
                    }
                }
                break;

            case STATE_READY:
               
                contexto.puntaje = 0;
                contexto.tiempo_restante = MAX_TIEMPO_PARTIDA;
                GameEvent_t inicio = { .tipo = START_GAME, .valor = contexto.nivel };
                //xSemaphoreGive(xServoSemaphore1);
                //xTaskNotify(xServoHandler, NOTIF_START, eSetBits);
                xQueueSend(xColaMovimiento, &inicio, 0);
                xQueueSend(xColaFeedback, &inicio, 0);
                xQueueSend(xColaComunicacion, &inicio, 0);
                xTaskNotifyGive(xComunicationHandler);
                 //uint32_t aux = (uint32_t)contexto.nivel;
                //xTaskNotify(xServoHandler, aux, eSetValueWithoutOverwrite);
                xTaskNotifyGive(xMatrizLDRHandler);
                xTimerStart(timerJuego, 0);
                
                contexto.estado = STATE_PLAYING;
                vTaskPrioritySet(xMatrizLDRHandler, tskIDLE_PRIORITY+4);
                vTaskPrioritySet(xFeedbackHandler, tskIDLE_PRIORITY+4);
                vTaskPrioritySet(xServoHandler, tskIDLE_PRIORITY+4);

                
                break;

            case STATE_PLAYING:
               //printf("Esperando evento en xColaJuego- STATE_PLAYING\r\n");
               //ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
                
               if (xQueueReceive(xColaJuego, &evento, portMAX_DELAY)) {
                     //printf("EVENTO RECIBIDO: %d\r\n", evento.tipo);
                    if (evento.tipo == DISPARO) {
                        uint16_t pts = GET_SCORE(evento.valor); //no sé si quieren hacer los puntos de acuerdo al nivel
                        contexto.puntaje += pts;
                        printf("[JUEGO] Disparo: +%u (Total=%u)\r\n", pts, contexto.puntaje);
                        GameEvent_t msg = { .tipo = DISPARO, .valor = contexto.puntaje };                 
                        
                        xQueueSend(xColaComunicacion, &msg, 0);   
                        xTaskNotifyGive(xComunicationHandler);    
                        
                        msg.valor = (pts == HIGHEST_SCORE);
                        xQueueSend(xColaFeedback, &msg, 0);   
                        
                    }
                    else if (evento.tipo == TICK_10MS) {
                        xQueueSend(xColaFeedback, &evento, 0);
                       //tm1637_show_number_dec_ex(&display, (int)evento.valor, dotsON, true, 4, 0);
                        static int counter=0;
                        if(!(++counter%100)){
                           if(!contexto.tiempo_restante) contexto.estado = STATE_GAME_OVER;
                           //printf("[JUEGO] Tiempo restante: %u \r\n", evento.valor/100);
                           GameEvent_t msg = { .tipo = TICK_1S, .valor = evento.valor/100 };
                           
                           xQueueSend(xColaComunicacion, &msg, 0);
                           xTaskNotifyGive(xComunicationHandler);
                           counter=0;
                        }
                        
                    }
                    else if (evento.tipo == GAME_OVER) {
                        contexto.estado = STATE_GAME_OVER;
                     
                    } else if (evento.tipo == GAME_RESET){
                        contexto.estado = STATE_GAME_RESET;
                       }
                    
                }
                vTaskDelay(pdMS_TO_TICKS(1));

                break;

            case STATE_GAME_OVER:
                printf("[JUEGO] Fin de partida! Puntaje: %u \r\n", contexto.puntaje);
                xTimerStop(timerJuego, 0);
               //tm1637_show_number_dec_ex(&display, 0, dotsON, true, 4, 0);
   
                GameEvent_t fin = { .tipo = FIN_PARTIDA, .valor = contexto.puntaje };
                xQueueSend(xColaFeedback, &fin, 0);
               // xTaskNotify(xServoHandler, NOTIF_STOP, eSetBits);
                xQueueSend(xColaMovimiento, &fin, 0);
                xQueueSend(xColaComunicacion, &fin, 0); 
                
                xTaskNotifyGive(xComunicationHandler);
               // xTaskNotifyGive(xServoHandler);
                xTaskNotifyGive(xMatrizLDRHandler);
                //printf("Esperando evento en xColaJuego - STATE_GAME_OVER \r\n");
                
                
               // contexto.estado = STATE_GAME_RESET;
                if (xQueueReceive(xColaJuego, &evento, portMAX_DELAY)) {
                    if (evento.tipo == GAME_RESET) {
                        xQueueSend(xColaComunicacion, &evento, 0);
                        xTaskNotifyGive(xComunicationHandler);
                        
                        xQueueReset(xColaJuego);
                        printf("[JUEGO] Reiniciando juego.\r\n");
                        contexto.estado = STATE_IDLE;
                    }
                }
                break;
            case STATE_GAME_RESET:
               printf("[JUEGO] Juego interrumpido por el usuario. Reiniciando\r\n");
                
               
                xTimerStop(timerJuego, 0);
                xTaskNotifyGive(xMatrizLDRHandler);
                //xTaskNotifyGive(xServoHandler);

                vTaskPrioritySet(xServoHandler, tskIDLE_PRIORITY);
                
                GameEvent_t reset = { .tipo = GAME_RESET, .valor = 0};
                xQueueSend(xColaFeedback, &reset, 0);
                xQueueSend(xColaMovimiento, &reset, 0);
                xQueueReset(xColaJuego);
                contexto.estado = STATE_IDLE;
                
            }
    }
}
