/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/15
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "userTasks.h"
#include "sapi_sct.h"
#include "semphr.h"
#include "servo.h"
#include "buzzer.h"
/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

extern TaskHandle_t xMatrizLDRHandler, xServoHandler, xJuegoHandler, xComunicationHandler, xFeedbackHandler;


SemaphoreHandle_t xServoSemaphoreDB = NULL, xAuxSemaphore = NULL;
/*=====[Main function, program entry point after power on or reset]==========*/


TimerHandle_t timer1;
#include "displayTM1637.h"
/*
volatile int t1 = 3000;TM1637Display display1;
static void TimerCallback(TimerHandle_t xTimer);

static void TimerCallback(TimerHandle_t xTimer)
{

    tm1637_show_number_dec_ex(&display1, t1--, dotsON, true, 4, 0);
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void tarea_init2(void *p) {
   
   tm1637_init(&display1, CLK, DIO, 5); // pinCLK=GPIO5, pinDIO=GPIO7
   tm1637_set_brightness(&display1, 7, true);
   tm1637_show_number_dec_ex(&display1, 0, dotsON, true, 4, 0);
   
   xTimerStart(timer1, 0);
   for(;;){
      if(!t1) t1=3000;
   }
}
*/
    
//acá ya se podría mandar a cada tarea los eventos que puede generar?
void tarea_init(void *p) {
   
   uint8_t servos[2] = {SERVO1_PIN, SERVO2_PIN};
   printf("Iniciando Servo %d y Servo %d \r\n", SERVO1_PIN, SERVO2_PIN);
   Servo_Init();
   for(;;){
      gpioWrite(LED1, ON);
       //Servo_Init();
      Servo_SetPosicionX(90);
      Servo_SetPosicionY(90);
      vTaskDelay(pdMS_TO_TICKS(200)); //xLastWakeTime se actualiza solo aca
      gpioWrite(LED1, OFF);
      gpioWrite(LED2, ON);
      Servo_SetPosicionX(180);
      Servo_SetPosicionY(180);
      vTaskDelay(pdMS_TO_TICKS(200)); //xLastWakeTime se actualiza solo aca
      gpioWrite(LED2, OFF);
      gpioWrite(LED3, ON);
      Servo_SetPosicionX( 0);
      Servo_SetPosicionY(0);
      soundStartGame();
      vTaskDelay(pdMS_TO_TICKS(200)); //xLastWakeTime se actualiza solo aca
      gpioWrite(LED3, OFF);
      
   }
}





int main( void ){
   // ----- Setup -----------------------------------
   boardInit();
   xServoSemaphoreDB = xSemaphoreCreateBinary();
   xAuxSemaphore = xSemaphoreCreateBinary();
   //Sct_Init(50); Sct_EnablePwmFor(CTOUT6); /*GPIO2*/Sct_EnablePwmFor(CTOUT7); //GPIO8
   //timer1 = xTimerCreate("TimerJuego", pdMS_TO_TICKS(10), pdTRUE, 0, TimerCallback);
   uartConfig(UART_USB, 115200);   // Inicializa UART por USB a 9600 baudios
   //printf("UART configurado a 115200 baudios\r\n");
   xColaJuego = xQueueCreate(20, sizeof(GameEvent_t));
   xColaMovimiento = xQueueCreate(10, sizeof(int));
   xColaFeedback = xQueueCreate(50, sizeof(int));
   xColaComunicacion = xQueueCreate(10, sizeof(int));
   
   //xTaskCreate(tarea_init, "INIT", 1024, NULL, 1, NULL);
   //xTaskCreate(tarea_init2, "INIT", 512, NULL, 1, NULL);
   

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

