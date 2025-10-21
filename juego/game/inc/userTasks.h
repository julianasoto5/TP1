#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

#include "sapi.h"



//Handlers
TaskHandle_t xMatrizLDRHandler, xServo1Handler, xJuegoHandler;
QueueHandle_t xColaJuego, xColaFeedback, xColaComunicacion, xColaMovimiento;

extern void matrizLDR_Init();

//Tareas
extern void tarea_sensores (void *);
extern void tarea_movimiento(void *);
extern void tarea_Juego (void *);
extern void tarea_feedBack(void*);
extern void tarea_comunicacion(void*);