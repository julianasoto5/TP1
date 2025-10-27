#ifndef SERVO_H_
#define SERVO_H_
#include "FreeRTOS.h"
#include <stdint.h>
#include <stddef.h>


#include "sapi.h"

//movimiento aleatorio
#include <time.h>
#include "stdlib.h"
#include "task.h" // Incluir si se usa vTaskDelay dentro del driver, o si se maneja desde la tarea

// Definiciones de Pines y Ángulos (Ajusta estos valores a tu EDU-CIAA)
#define ANGULO_MIN_GRADOSX 60
#define ANGULO_MAX_GRADOSX 120
#define ANGULO_MIN_GRADOSY 45
#define ANGULO_MAX_GRADOSY 135
#define POS_INICIAL_GRADOS 90

// Pines del Hardware
#define SERVO1_PIN SERVO4 //GPIO8
#define SERVO2_PIN SERVO8 //GPIO2 

// Funciones de control de la diana
void Servo_Init(void);
void Servo_SetPosicionX(uint8_t angle);
void Servo_SetPosicionY(uint8_t angle);
void Servo_PonerInicial(void);
int16_t Servo_GetPosicion(servoMap_t servo); 
void Servo_MoverGradual(servoMap_t servo, int16_t destino, uint16_t velocidad_ms);

#endif //SERVO_H_