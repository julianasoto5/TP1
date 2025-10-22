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
   printf("ServosInit()\r\n");
   srand(time(NULL));
   bool_t valor = 0;
   printf("ServosInit()\r\n");
   // Configurar Servo
   valor = servoConfig( 0, SERVO_ENABLE ); //inicializa timers
   printf("ServosInit()\r\n");

   //agrega el servo a la lista de servos activos - devuelve 0 si no tuvo éxito
   if (!servoConfig( SERVO1_PIN, SERVO_ENABLE_OUTPUT ))
      printf("No se pudo agregar el servo 1 a la lista de servos activos.\r\n");
   printf("ServosInit()\r\n");

   if(!servoConfig( SERVO2_PIN, SERVO_ENABLE_OUTPUT ))
      printf("No se pudo agregar el servo 2 a la lista de servos activos.\r\n");
   printf("ServosInit()\r\n");
   
   // Usar Servo
   valor = servoWrite( SERVO1_PIN, POS_INICIAL_SERVO1);
   valor = servoWrite( SERVO2_PIN, POS_INICIAL_SERVO2);
   printf("ServosInit()\r\n");
   
   //servoAngle = servoRead( SERVO_N );
}

void moveServo_Random(uint8_t servo){
   //uint8_t servoAngle = servoRead( servo );
   uint8_t movement = rand() % 180;//GET_MAX_ANGLE(servo); //esto lo tengo que ver porque de acuerdo a cada servo probablemente tenga un MAX distinto
   servoWrite(servo, movement);
}

void moveServo(uint8_t servo){
   uint8_t servoAngle = servoRead( servo );
   uint8_t movement = servoAngle + 0;//tendría que seguir el movimiento adelante o atrás --ver
   servoWrite(servo, movement);
}
