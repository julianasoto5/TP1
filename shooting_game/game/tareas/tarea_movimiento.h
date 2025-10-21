#ifndef TAREA_MOVIMIENTO_H_
#define TAREA_MOVIMIENTO_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "tarea_juego.h" // Necesario para GameEvent_t y tipos de cola


void Tarea_Movimiento(void *pvParameters);

#endif /* TAREA_MOVIMIENTO_H_ */