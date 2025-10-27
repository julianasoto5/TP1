#include "tarea_juego.h" // Necesario para GameEvent_t y tipos de cola
#include "task.h"
#include <stdio.h>

extern QueueHandle_t xColaFeedback; 
 // <-- Importante para acceder al objeto

// Define el bitmask del colon
// TÍPICAMENTE es el bit 6: 0x40 (0b01000000). Ajústalo si no funciona.
#define COLON_BITMASK 0b01000000
// Funciones de Control de Drivers 

static void EncenderSecuenciaLEDsInicio(void){
    printf("[FEEDBACK] -> Secuencia LEDs INICIO (Drivers)\r\n");
}

static void ReproducirSonidoInicio(void){
    printf("[FEEDBACK] -> Sonido INICIO (Drivers)\r\n");
}

static void EncenderLEDsVerdes(void){
    printf("[FEEDBACK] -> LEDs VERDES (Drivers)\r\n");
}

static void ReproducirSonidoAcierto(void){
    printf("[FEEDBACK] -> Sonido ACIERTO (Drivers)\r\n");
}


static void MostrarTiempoLCD(uint8_t tiempo_restante){
    printf("[FEEDBACK] -> Mostrar en LCD Tiempo: %u \r\n", tiempo_restante);
   /*int num_a_mostrar = (int)tiempo_restante;
   display.showNumberDecEx(
        num_a_mostrar, 
        COLON_BITMASK,  // Encender el colon (separador)
        true,           // Usar ceros a la izquierda (leading_zero = true -> Muestra "0020")
        4,              // Usar los 4 dígitos
        0               // Empezar desde la posición 0 (izquierda)
    );*/
}

static void EncenderSecuenciaLEDsFinal(void){
    printf("[FEEDBACK] -> Secuencia LEDs FINAL (Drivers)\r\n");
}

static void ReproducirSonidoFinal(void){
    printf("[FEEDBACK] -> Sonido FINAL (Drivers)\r\n");
}

/* Tarea principal */

void tarea_feedback(void *pvParameters)
{
    GameEvent_t evento;

    while (1) {
        // Loop: Esperar evento desde la tarea de Juego 
        if (xQueueReceive(xColaFeedback, &evento, portMAX_DELAY) == pdPASS) {
            
            if (evento.tipo == START_GAME) {
                EncenderSecuenciaLEDsInicio(); 
                ReproducirSonidoInicio();      
            }
            else if (evento.tipo == DISPARO) {
                EncenderLEDsVerdes();         
                ReproducirSonidoAcierto();     
            }
            else if (evento.tipo == TICK_1S) {
                MostrarTiempoLCD(evento.valor); // La tarea juego envía el tiempo en valor
            }
           
            else if (evento.tipo == FIN_PARTIDA) {
                EncenderSecuenciaLEDsFinal(); 
                ReproducirSonidoFinal();     
            }
        }
    }
}