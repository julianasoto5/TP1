#include "servo.h"


void Servo_Init() {
   srand(time(NULL));
   bool_t valor = 0;
   // Configurar Servo
   valor = servoConfig( 0, SERVO_ENABLE ); //inicializa timers

   //agrega el servo a la lista de servos activos - devuelve 0 si no tuvo éxito
   if (!servoConfig( SERVO1_PIN, SERVO_ENABLE_OUTPUT ))
      printf("No se pudo agregar el servo 1 a la lista de servos activos.\r\n");

   if(!servoConfig( SERVO2_PIN, SERVO_ENABLE_OUTPUT ))
      printf("No se pudo agregar el servo 2 a la lista de servos activos.\r\n");
   
   Servo_PonerInicial();
   
   //servoAngle = servoRead( SERVO_N );
}
int16_t Servo_GetPosicion(servoMap_t servo) {
    return (int16_t)servoRead(servo);
}
// --- Implementación de Servo_MoverGradual ---
void Servo_MoverGradual(servoMap_t servo, int16_t destino, uint16_t velocidad_ms) {
    int16_t actual = Servo_GetPosicion(servo); 
    int16_t paso_unidades = (destino > actual) ? 1 : -1;
    
    const TickType_t RETARDO_PASO_MS = pdMS_TO_TICKS(15); 
    
    
    
    while (actual != destino) {
        
        // Mover un paso
        actual += paso_unidades;
        
        if (servo == SERVO1_PIN) {
            Servo_SetPosicionX(actual);
        } else if (servo == SERVO2_PIN) {
            Servo_SetPosicionY(actual);
        }

        // Cede la CPU: Esto es lo que hace el movimiento fluido.
        vTaskDelay(RETARDO_PASO_MS); 
        
        // Chequeo para evitar bucles infinitos por errores de precisión
        if ((paso_unidades > 0 && actual >= destino) || (paso_unidades < 0 && actual <= destino)) {
            break;
        }
    }
}


void Servo_SetPosicionX(uint8_t angle) {
    if (angle < ANGULO_MIN_GRADOSX) angle = ANGULO_MIN_GRADOSX;
    if (angle > ANGULO_MAX_GRADOSX) angle = ANGULO_MAX_GRADOSX;
    
    servoWrite(SERVO1_PIN, angle);
}

void Servo_SetPosicionY(uint8_t angle) {
    if (angle < ANGULO_MIN_GRADOSY) angle = ANGULO_MIN_GRADOSY;
    if (angle > ANGULO_MAX_GRADOSY) angle = ANGULO_MAX_GRADOSY;
    servoWrite(SERVO2_PIN, angle);
}

void Servo_PonerInicial(void) {
   Servo_SetPosicionX(POS_INICIAL_GRADOS);
   Servo_SetPosicionY(POS_INICIAL_GRADOSY);

}