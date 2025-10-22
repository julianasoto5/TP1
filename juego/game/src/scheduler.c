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

extern TaskHandle_t xMatrizLDRHandler, xServo1Handler, xJuegoHandler, xComunicationHandler, xFeedbackHandler;


/*=====[Main function, program entry point after power on or reset]==========*/

//acá ya se podría mandar a cada tarea los eventos que puede generar?
int main( void )
{
   // ----- Setup -----------------------------------
   boardInit();
   uartConfig(UART_USB, 9600);   // Inicializa UART por USB a 9600 baudios

   xColaJuego = xQueueCreate(10, sizeof(GameEvent_t));
   xColaMovimiento = xQueueCreate(4, sizeof(int));
   xColaFeedback = xQueueCreate(4, sizeof(int));
   xColaComunicacion = xQueueCreate(4, sizeof(int));
   
   xTaskCreate( tarea_sensores, "Tarea Sensores", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xMatrizLDRHandler );
   xTaskCreate( tarea_juego, "Tarea Juego", configMINIMAL_STACK_SIZE*3, NULL, tskIDLE_PRIORITY+2, &xJuegoHandler);
   xTaskCreate( tarea_movimiento, "Tarea Movimiento", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &xServo1Handler);
   //xTaskCreate( tarea_movimiento, "Tarea Movimiento motor 2", configMINIMAL_STACK_SIZE, &(pin2), tskIDLE_PRIORITY+1, &xServo2Handler);
   //xTaskCreate( tarea_comunicacion, "Tarea Comunicacion", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xComunicationHandler);
   //xTaskCreate( tarea_feedback, "Tarea Feedback", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xFeedbackHandler);
   
   
   vTaskStartScheduler();
   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}

