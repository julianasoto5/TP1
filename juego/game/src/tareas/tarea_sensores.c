#include "matrizLDR.h"
#include "userTasks.h"
#include "tarea_juego.h"

extern QueueHandle_t xColaJuego;

void tarea_sensores(void * prm){ //prioridad media? 2/3 
const TickType_t xDelay10ms = pdMS_TO_TICKS( 10 );    
TickType_t xLastWakeTime = xTaskGetTickCount(); //esto garantiza que se despierte cada 10ms porque sino depende de cuando llame a vTaskDelay
UBaseType_t uxPriority = uxTaskPriorityGet( NULL ); //con NULL devuelve la propia
   
GameEvent_t event;
   event.tipo = DISPARO;//*(GameEvent_t*) prm; //ya precargado con tipo o mandar solo tipo
/*
   typedef struct {
    GameEventType_t tipo;
    uint8_t valor;   // nivel, anillo, tiempo, puntaje, etc.
} GameEvent_t;
*/ 
   matrizLDR_Init();
   uint8_t ring;
   for(;;){
         if(matrizLDR_Scan(&ring)) {
           printf("Se manda a xColaJuego\r\n");
           event.valor = ring; 
           xQueueSend(xColaJuego, &event, 0); 
         }
         
         //delay para que ADC se estabilice
         vTaskDelayUntil(&xLastWakeTime, xDelay10ms); //xLastWakeTime se actualiza solo aca
   }
}