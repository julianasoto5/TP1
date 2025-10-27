#include "tarea_juego.h"
#include "userTasks.h"
#include <stdio.h>

extern QueueHandle_t xColaJuego;
extern QueueHandle_t xColaFeedback;
extern QueueHandle_t xColaComunicacion;
extern QueueHandle_t xColaMovimiento;
TimerHandle_t timerJuego;

static GameContext_t contexto;

/* ---- Configuración general ---- */

static void TimerCallbackJuego(TimerHandle_t xTimer);


void Juego_Init(void)
{
    timerJuego = xTimerCreate("TimerJuego", pdMS_TO_TICKS(1000), pdTRUE, 0, TimerCallbackJuego); //deberia ser cada 10ms porque el display va a tener esa resolucion

    contexto.estado = STATE_IDLE;
    contexto.nivel = NIVEL_DIFICIL;
    contexto.puntaje = 0;
    contexto.tiempo_restante = MAX_TIEMPO_PARTIDA;
}

/* Envío de eventos desde tareas  */
void Juego_EnviarEvento(GameEventType_t tipo, uint8_t val)
{
    GameEvent_t evento = { .tipo = tipo, .valor = val };
    xQueueSend(xColaJuego, &evento, 0);
}

/* Callback del timer de 1s */
static void TimerCallbackJuego(TimerHandle_t xTimer)
{
    GameEvent_t evento = {.tipo = TICK_1S, .valor = 0 };
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(xColaJuego, &evento, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* Función auxiliar: cálculo de puntaje */
static uint16_t CalcularPuntaje(uint8_t nivel, uint8_t ring)
{
    const uint16_t base[5] = {100, 70, 50, 30, 10};
    uint8_t mult = 1 + (nivel - 1);
    if (ring > 4) ring = 4;
    return base[ring] * mult;
}

/* Máquina de estados  */
void tarea_juego(void *pvParameters)
{
   GameEvent_t evento;
   Juego_Init();
    while(1) {
            switch (contexto.estado) {

            case STATE_IDLE:
                contexto.puntaje = 0;
                contexto.tiempo_restante = MAX_TIEMPO_PARTIDA;
                printf("Esperando evento en xColaJuego - STATE_IDLE\r\n");
                if (xQueueReceive(xColaJuego, &evento, portMAX_DELAY)) {
                    if (evento.tipo == SELECT_LEVEL) {
                        contexto.nivel = evento.valor;
                    }
                    else if (evento.tipo == START_GAME) {
                        contexto.estado = STATE_READY;
                    }
                }
                break;

            case STATE_READY:

                contexto.puntaje = 0;
                contexto.tiempo_restante = MAX_TIEMPO_PARTIDA;
                GameEvent_t inicio = { .tipo = START_GAME, .valor = contexto.nivel };

                xQueueSend(xColaMovimiento, &inicio, 0);
                xQueueSend(xColaFeedback, &inicio, 0);
                //xQueueSend(xColaComunicacion, &inicio, 0);

                xTimerStart(timerJuego, 0);
                contexto.estado = STATE_PLAYING;
                break;

            case STATE_PLAYING:
               printf("Esperando evento en xColaJuego- STATE_PLAYING\r\n");
               if (xQueueReceive(xColaJuego, &evento, portMAX_DELAY)) {
                    if (evento.tipo == DISPARO) {
                        uint16_t pts = GET_SCORE(evento.valor); //no sé si quieren hacer los puntos de acuerdo al nivel
                        contexto.puntaje += pts;
                        printf("[JUEGO] Disparo: +%u (Total=%u)\r\n", pts, contexto.puntaje);
                        GameEvent_t msg = { .tipo = DISPARO, .valor = contexto.puntaje };                        
                        xQueueSend(xColaFeedback, &msg, 0);                        
                        xQueueSend(xColaComunicacion, &msg, 0);
                    }
                    else if (evento.tipo == TICK_1S) {
                        if (contexto.tiempo_restante > 0)
                            contexto.tiempo_restante--;

                        printf("[JUEGO] Tiempo restante: %u \r\n", contexto.tiempo_restante);

                        GameEvent_t msg = { .tipo = TICK_1S, .valor = contexto.tiempo_restante };
                        xQueueSend(xColaFeedback, &msg, 0);
                        xQueueSend(xColaComunicacion, &msg, 0);

                        if (contexto.tiempo_restante == 0) {
                            contexto.estado = STATE_GAME_OVER;
                        }
                    }
                    else if (evento.tipo == GAME_OVER) {
                        contexto.estado = STATE_GAME_OVER;
                     
                    }         
                }
                break;

            case STATE_GAME_OVER:
                printf("[JUEGO] Fin de partida! Puntaje: %u \r\n", contexto.puntaje);
                xTimerStop(timerJuego, 0);
                GameEvent_t fin = { .tipo = FIN_PARTIDA, .valor = contexto.puntaje };
                xQueueSend(xColaFeedback, &fin, 0);
                xQueueSend(xColaMovimiento, &fin, 0);
                printf("Esperando evento en xColaJuego - STATE_GAME_OVER \r\n");
                if (xQueueReceive(xColaJuego, &evento, portMAX_DELAY)) {
                    if (evento.tipo == GAME_RESET) {
                        xQueueReset(xColaJuego);
                        printf("[JUEGO] Reiniciando juego.\r\n");
                        contexto.estado = STATE_IDLE;
                    }
                }
                break;
            }
        
    }
}