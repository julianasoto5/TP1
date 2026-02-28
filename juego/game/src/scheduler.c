/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/15
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "userTasks.h"
/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

extern TaskHandle_t xMatrizLDRHandler, xServoHandler, xJuegoHandler, xComunicationHandler, xFeedbackHandler;
/*=====[Main function, program entry point after power on or reset]==========*/


int main( void ){
   // ----- Setup -----------------------------------
   boardInit();
   
   uartConfig(UART_USB, 115200);   // Inicializa UART por USB a 9600 baudios
   //printf("UART configurado a 115200 baudios\r\n");
   xColaJuego = xQueueCreate(20, sizeof(GameEvent_t));
   xColaMovimiento = xQueueCreate(10, sizeof(int));
   xColaFeedback = xQueueCreate(50, sizeof(int));
   xColaComunicacion = xQueueCreate(10, sizeof(int));
   

   xTaskCreate( tarea_juego, "Tarea Juego", configMINIMAL_STACK_SIZE*3, NULL, tskIDLE_PRIORITY+4, &xJuegoHandler);
   xTaskCreate( tarea_movimiento, "Tarea Movimiento", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, &xServoHandler);
   xTaskCreate( tarea_comunicacion, "Tarea Comunicacion", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, &xComunicationHandler);
   xTaskCreate( tarea_feedback, "Tarea Feedback", configMINIMAL_STACK_SIZE*4, NULL, tskIDLE_PRIORITY+3, &xFeedbackHandler);
   xTaskCreate( tarea_sensores, "Tarea Sensores", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, &xMatrizLDRHandler );
   
   vTaskStartScheduler();
   

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}

