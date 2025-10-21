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

const char *pcTextForTask1 = "Tarea Sensores está corriendo\r\n";
const char *pcTextForTask2 = "Tarea Motor1 está corriendo\r\n";

extern TaskHandle_t xMatrizLDRHandler, xServo1Handler, xJuegoHandler;


/*=====[Main function, program entry point after power on or reset]==========*/
//taskENABLE_INTERRUPTS(void);
int main( void )
{
   // ----- Setup -----------------------------------
   boardInit();
   uartConfig(UART_USB, 9600);   // Inicializa UART por USB a 9600 baudios
      matrizLDR_Init();

   xColaJuego = xQueueCreate(4, sizeof(int));
    /* Create the first task at priority 1... */
   xTaskCreate( tarea_sensores, "Tarea Sensores", configMINIMAL_STACK_SIZE*4, NULL, tskIDLE_PRIORITY+1, &xMatrizLDRHandler );
   xTaskCreate( tarea_Juego, "Tarea Juego", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xJuegoHandler);
  
   
   
   vTaskStartScheduler();
   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}

