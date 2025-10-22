#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

#include "sapi.h"

#include "tarea_juego.h"

//Handlers
TaskHandle_t xMatrizLDRHandler, xServo1Handler, xServo2Handler, xJuegoHandler, xComunicationHandler, xFeedbackHandler;
QueueHandle_t xColaJuego, xColaFeedback, xColaComunicacion, xColaMovimiento;

extern void matrizLDR_Init();

//Tareas
extern void tarea_sensores (void *);
extern void tarea_movimiento(void *);
extern void tarea_juego (void *);
extern void tarea_feedback(void*);
extern void tarea_comunicacion(void*);