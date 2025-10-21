#include "servo.h"
#include "userTasks.h"

//va a tener dos instancias -- servo1 y servo2 - se podria ejecutar constantemente en vez de periodicamente.
void tarea_movimiento(void *prm){ //prioridad media? 2/3 
const TickType_t xDelay15ms = pdMS_TO_TICKS( 15 );
TickType_t xLastWakeTime = xTaskGetTickCount(); 
uint8_t servoPin = *(uint8_t*) prm;
   servosInit();
   for(;;){
      moveServo_Random(servoPin);   
      vTaskDelayUntil(&xLastWakeTime,xDelay15ms);
   }
}