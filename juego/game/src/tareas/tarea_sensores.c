#include "matrizLDR.h"
#include "userTasks.h"
#include "tarea_juego.h"

#include <stdlib.h> // Necesario para rand() y abs()

extern QueueHandle_t xColaJuego;
extern TaskHandle_t xMatrizLDRHandler, xServoHandler, xJuegoHandler, xComunicationHandler, xFeedbackHandler;

void tarea_sensores(void * prm){ //prioridad media? 2/3 
   const TickType_t xDelay50ms = pdMS_TO_TICKS( 50 );    
   TickType_t xLastWakeTime = xTaskGetTickCount(); //esto garantiza que se despierte cada 10ms porque sino depende de cuando llame a vTaskDelay
      
   GameEvent_t event;
   event.tipo = DISPARO;//*(GameEvent_t*) prm; //ya precargado con tipo o mandar solo tipo

   matrizLDR_Init();
   uint8_t ring;
   int8_t dormir = 1;
   for(;;){
      
      //printf(".");
      if(dormir){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        dormir = 0;
      }
      else{
         //printf("[SENSOR] Prioridad %d, [JUEGO] Prioridad %d, [COM] Prioridad %d, [FB] Prioridad %d\r\n", uxTaskPriorityGet( NULL ),uxTaskPriorityGet(xJuegoHandler), uxTaskPriorityGet(xComunicationHandler), uxTaskPriorityGet(xFeedbackHandler));
         
         if(matrizLDR_Scan(&ring)) {
              //printf("Se manda a xColaJuego\r\n");
              event.valor = ring; 
              xQueueSend(xColaJuego, &event, 0); 
         }
            
            //delay para que ADC se estabilice
         vTaskDelayUntil(&xLastWakeTime, xDelay50ms); //xLastWakeTime se actualiza solo aca
         
         if(ulTaskNotifyTake(pdTRUE, 0)){
            dormir = 1;
   
         }
      }
   }
}