#include "servo.h"
#include "userTasks.h"
#include "tarea_juego.h"
extern QueueHandle_t xColaMovimiento;
//va a tener dos instancias -- servo1 y servo2 - se podria ejecutar constantemente en vez de periodicamente.


void moveNivelDificil(){
   
}

void moveNivelFacil(){
   
}
void* levels[2] = {moveNivelFacil, moveNivelDificil};
void tarea_movimiento(void *prm){ //prioridad media? 2/3 
const TickType_t xDelay15ms = pdMS_TO_TICKS( 15 );
TickType_t xLastWakeTime = xTaskGetTickCount(); 
GameEvent_t evento;
   printf("Iniciando servos\r\n");
   servosInit();
   for(;;){
      
      printf("Esperando evento en xColaMovimiento\r\n");
      if (xQueueReceive(xColaMovimiento, &evento, portMAX_DELAY)){
         switch (evento.tipo){
            case START_GAME: levels[evento.valor]; break; 
            case DISPARO: break;  //no creo que haya que hacer algo distinto aca
            case FIN_PARTIDA: break; //se deberia implementar con ISR?
         }
      }
      
      //moveServo_Random();   
      vTaskDelayUntil(&xLastWakeTime,xDelay15ms);
   }
}

