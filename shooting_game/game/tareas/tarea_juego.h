#ifndef TAREAS_JUEGO_H_
#define TAREAS_JUEGO_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"
#include <stdint.h>

typedef enum { NONE, SELECT_LEVEL, START_GAME, DISPARO, TICK_1S, FIN_PARTIDA, RESET } GameEventType_t;

typedef enum { STATE_IDLE, STATE_READY, STATE_PLAYING, STATE_GAME_OVER } GameState_t;

/* ---- Estructura del evento ---- */
typedef struct {
    GameEventType_t tipo;
    uint8_t valor;   // nivel, anillo, tiempo, puntaje, etc.
} GameEvent_t;

typedef struct {
    GameState_t estado;
    uint8_t nivel;
    uint16_t puntaje;
    uint16_t tiempo_restante;
} GameContext_t;


/* ---- Recursos compartidos ---- */
extern QueueHandle_t colaJuego;          // recibe eventos desde sensores / comunicación
extern QueueHandle_t colaFeedback;       // envía señales a LEDs/display
extern QueueHandle_t colaComunicacion;   // envía datos al módulo WiFi
extern TaskHandle_t handleMovimiento;    // se usa para notificar inicio/stop

/* ---- Funciones públicas ---- */
void Juego_Init(void);
void Juego_EnviarEvento(GameEventType_t tipo, uint8_t val);
void Tarea_Juego(void *pvParameters);

#endif
