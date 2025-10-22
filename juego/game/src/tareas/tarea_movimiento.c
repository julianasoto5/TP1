#include "servo.h"
#include "userTasks.h"
#include "tarea_juego.h"
#include "task.h"
#include <stdio.h>
#include <stdbool.h>
extern QueueHandle_t xColaMovimiento;



/* Funciones de control de motores (Mockup) */

void PonerDianaPosicionInicial(void) {
    servoWrite(SERVO1_PIN, POS_INICIAL_SERVO1);
    servoWrite(SERVO2_PIN, POS_INICIAL_SERVO2);
}


void MoverDianaFacil(void) {
    printf("[MOVIMIENTO] -> Moviendo diana en patron FACIL.\n");
    // Lógica
}


void MoverDianaDificil(void) {
    printf("[MOVIMIENTO] -> Moviendo diana en patron DIFICIL/Aleatorio.\n");
    // Lógica
}

void* niveles[2] = {MoverDianaFacil, MoverDianaDificil};
/* Tarea Principal */

void tarea_movimiento(void *pvParameters)
{
    GameEvent_t msg;
    uint8_t nivel = 0;
    bool moviendo = false;

    // Inicialmente, la tarea espera el mensaje de la Tarea Juego (en READY)
    while (1) {


        if (!moviendo) {
            printf("[MOVIMIENTO] En espera de evento START (NIVEL).\n");
            // Esperar un mensaje de incio de la tarea Juego (que en READY enviará el nivel) 
            if (xQueueReceive(xColaMovimiento, &msg, portMAX_DELAY) == pdPASS) {
                // El evento de INICIO se envía como START_GAME con el nivel en 'valor'.
                if (msg.tipo == START_GAME) {
                    nivel = msg.valor;
                    moviendo = true;
                    printf("[MOVIMIENTO] Partida iniciada. Nivel: %u. Iniciando movimiento.\n", nivel);
                }
            }
        }
        
        // Bucle principal de movimiento cuando ya inicio el juego.
        if (moviendo) {
            // Se comprueba si hay un mensaje STOP (sin bloquear)- La Tarea Juego envía un mensaje 'FIN' para indicar STOP
            if (xQueueReceive(xColaMovimiento, &msg, 0) == pdPASS) { 
                if (msg.tipo == FIN_PARTIDA) { 
                    printf("[MOVIMIENTO] Evento STOP recibido. Deteniendo movimiento.\n");
                    PonerDianaPosicionInicial(); // Poner la diana en la posición inicial 
                    moviendo = false;
                    continue; 
                }
            }
            niveles[msg.valor];
         } 
   }
}
