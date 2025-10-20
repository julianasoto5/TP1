/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/17
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "servo.h"


/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

void servosInit(){
   //Se hace solo aca?
   srand(time(NULL));
   bool_t valor = 0;

   uint8_t servo1Angle = POS_INICIAL_SERVO1; // 0 a 180 grados
   uint8_t servo2Angle = POS_INICIAL_SERVO2; // 0 a 180 grados
   // Configurar Servo
   valor = servoConfig( 0, SERVO_ENABLE ); //inicializa timers

   //agrega el servo a la lista de servos activos - devuelve 0 si no tuvo éxito
   if (!servoConfig( SERVO1_PIN, SERVO_ENABLE_OUTPUT ))
      printf("No se pudo agregar el servo 1 a la lista de servos activos.\r\n");
   if(!servoConfig( SERVO2_PIN, SERVO_ENABLE_OUTPUT ))
      printf("No se pudo agregar el servo 2 a la lista de servos activos.\r\n");
   
   // Usar Servo
   valor = servoWrite( SERVO1_PIN, servo1Angle );
   valor = servoWrite( SERVO2_PIN, servo2Angle );
   
   //servoAngle = servoRead( SERVO_N );
}

void moveServo(uint8_t servo){
   //uint8_t servoAngle = servoRead( servo );
   uint8_t movement = rand() % 180;
   servoWrite(servo, movement);
}


int main( void )
{
   // ----- Setup -----------------------------------
   boardInit();

   servosInit();
   
   // ----- Repeat for ever -------------------------
   while( true ) {
      moveServo(SERVO1_PIN);
      moveServo(SERVO2_PIN);
   }

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}
