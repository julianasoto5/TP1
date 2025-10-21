#include "matrizLDR.h"
#include "userTasks.h"

extern QueueHandle_t xColaJuego;

void tarea_sensores(void * prm){ //prioridad media? 2/3 
const TickType_t xDelay10ms = pdMS_TO_TICKS( 10 );    
TickType_t xLastWakeTime = xTaskGetTickCount(); //esto garantiza que se despierte cada 10ms porque sino depende de cuando llame a vTaskDelay
UBaseType_t uxPriority = uxTaskPriorityGet( NULL ); //con NULL devuelve la propia
int ring;
   
   
   for(;;){
         ring = muestreoLDR();
         if(ring) xQueueSend(xColaJuego, &ring, 0);
         //delay para que ADC se estabilice
         vTaskDelayUntil(&xLastWakeTime, xDelay10ms); //xLastWakeTime se actualiza solo aca
   }
}