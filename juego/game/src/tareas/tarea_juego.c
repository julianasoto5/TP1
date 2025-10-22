#include "tarea_juego.h"
#include "userTasks.h"
#include <stdio.h>

extern QueueHandle_t xColaJuego;
extern QueueHandle_t xColaFeedback;
extern QueueHandle_t xColaComunicacion;
extern QueueHandle_t xColaMovimiento;
//TaskHandle_t handleMovimiento;
TimerHandle_t timerJuego;

static GameContext_t contexto;

/* ---- Configuración general ---- */
#define MAX_TIEMPO_PARTIDA 60

static void TimerCallbackJuego(TimerHandle_t xTimer);
//static uint16_t CalcularPuntaje(uint8_t nivel, uint8_t ring);


void Juego_Init(void)
{

    // Las colas deben estar creadas en el main
    timerJuego = xTimerCreate("TimerJuego", pdMS_TO_TICKS(1000), pdTRUE, 0, TimerCallbackJuego); //deberia ser cada 10ms porque el display va a tener esa resolucion

    contexto.estado = STATE_PLAYING;
    contexto.nivel = NIVEL_FACIL;
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
                printf("Esperando evento en xColaJuego - STATE_IDLE\r\n");
                if (xQueueReceive(xColaJuego, &evento, portMAX_DELAY)) {
                    if (evento.tipo == SELECT_LEVEL) {
                        contexto.nivel = evento.valor;
                        printf("[JUEGO] Nivel %d seleccionado\r\n", contexto.nivel);
                    }
                    else if (evento.tipo == START_GAME) {
                        printf("[JUEGO] Iniciando partida...\r\n");
                        contexto.estado = STATE_READY;
                    }
                }
                break;

            case STATE_READY:
                contexto.puntaje = 0;
                contexto.tiempo_restante = MAX_TIEMPO_PARTIDA;

                // Feedback y comunicación
                GameEvent_t inicio = { .tipo = START_GAME, .valor = contexto.nivel };

                xQueueSend(xColaMovimiento, &inicio, 0);
                xQueueSend(xColaFeedback, &inicio, 0);
                //xQueueSend(colaComunicacion, &fb, 0);

                // Iniciar timer
                xTimerStart(timerJuego, 0);
                contexto.estado = STATE_PLAYING;
                break;

            case STATE_PLAYING:
               printf("Esperando evento en xColaJuego- STATE_PLAYING\r\n");
                if (xQueueReceive(xColaJuego, &evento, portMAX_DELAY)) {
                    if (evento.tipo == DISPARO) {
                        //uint16_t pts = CalcularPuntaje(contexto.nivel, evento.valor);
                        uint16_t pts = GET_SCORE(evento.valor); //no sé si quieren hacer los puntos de acuerdo al nivel
                        contexto.puntaje += pts;
                        //contexto.puntaje += CalcularPuntaje(contexto.nivel, evento.valor);
                        printf("[JUEGO] Disparo: +%u (Total=%u)\r\n", pts, contexto.puntaje);

                        
                        GameEvent_t msg = { .tipo = DISPARO, .valor = contexto.puntaje };
                        //Indicar que hubo un acierto a la tarea feedback
                        
                        xQueueSend(xColaFeedback, &msg, 0);
                        //Enviar puntaje actualizado a la tarea de comunicación
                        
                        xQueueSend(xColaMovimiento, &msg, 0);
                    }
                    else if (evento.tipo == TICK_1S) {
                        if (contexto.tiempo_restante > 0)
                            contexto.tiempo_restante--;

                        printf("[JUEGO] Tiempo restante: %u\n", contexto.tiempo_restante);

                        GameEvent_t msg = { .tipo = TICK_1S, .valor = contexto.tiempo_restante };
                        //Enviar tiempo actualizado a las tareas feedback y comunicación
                        xQueueSend(xColaFeedback, &msg, 0);
                        xQueueSend(xColaComunicacion, &msg, 0);

                        if (contexto.tiempo_restante == 0) {
                            contexto.estado = STATE_GAME_OVER;
                        }
                    }
                }
                break;

            case STATE_GAME_OVER:
                printf("[JUEGO] Fin de partida! Puntaje: %u\r\n", contexto.puntaje);
                xTimerStop(timerJuego, 0);

                GameEvent_t fin = { .tipo = FIN_PARTIDA, .valor = contexto.puntaje };
                //Indicar que termino el juego y enviar puntaje final a las tareas feedback y comunicación
                xQueueSend(xColaFeedback, &fin, 0);
                xQueueSend(xColaComunicacion, &fin, 0);
                // Enviar STOP a Tarea Movimiento 
                xQueueSend(xColaMovimiento, &fin, 0);
                printf("Esperando evento en xColaJuego - STATE_GAME_OVER\r\n");
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