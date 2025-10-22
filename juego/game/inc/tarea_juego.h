#ifndef TAREAS_JUEGO_H_
#define TAREAS_JUEGO_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdint.h>
#include "math.h"

#define CANT_ANILLOS 2
#define HIGHEST_SCORE 100
#define BASE 2
#define GET_SCORE(anillo) HIGHEST_SCORE*pow(BASE, anillo-CANT_ANILLOS) //debe dividir
   
   
typedef enum { NONE, SELECT_LEVEL, START_GAME, DISPARO, TICK_1S, FIN_PARTIDA, GAME_RESET } GameEventType_t;

typedef enum { STATE_IDLE, STATE_READY, STATE_PLAYING, STATE_GAME_OVER } GameState_t;

typedef enum {NIVEL_FACIL, NIVEL_DIFICIL} level;

/* ---- Estructura del evento ---- */
typedef struct {
    GameEventType_t tipo;
    uint8_t valor;   // nivel, anillo, tiempo, puntaje, etc.
} GameEvent_t;

typedef struct {
    GameState_t estado;
    level nivel;
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
void tarea_Juego(void *pvParameters);

#endif
