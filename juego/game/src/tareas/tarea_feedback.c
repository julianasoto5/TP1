#include "tarea_juego.h" // Necesario para GameEvent_t y tipos de cola
#include "task.h"
#include <stdio.h>

extern QueueHandle_t colaFeedback; 

// Funciones de Control de Drivers 

static void EncenderSecuenciaLEDsInicio(void){
    printf("[FEEDBACK] -> Secuencia LEDs INICIO (Drivers)\n");
}

static void ReproducirSonidoInicio(void){
    printf("[FEEDBACK] -> Sonido INICIO (Drivers)\n");
}

static void EncenderLEDsVerdes(void){
    printf("[FEEDBACK] -> LEDs VERDES (Drivers)\n");
}

static void ReproducirSonidoAcierto(void){
    printf("[FEEDBACK] -> Sonido ACIERTO (Drivers)\n");
}


static void MostrarTiempoLCD(uint8_t tiempo_restante){
    printf("[FEEDBACK] -> Mostrar en LCD Tiempo: %u s\n", tiempo_restante);
}

static void EncenderSecuenciaLEDsFinal(void){
    printf("[FEEDBACK] -> Secuencia LEDs FINAL (Drivers)\n");
}

static void ReproducirSonidoFinal(void){
    printf("[FEEDBACK] -> Sonido FINAL (Drivers)\n");
}

/* Tarea principal */

void tarea_feedback(void *pvParameters)
{
    GameEvent_t evento;

    while (1) {
        // Loop: Esperar evento desde la tarea de Juego 
        if (xQueueReceive(colaFeedback, &evento, portMAX_DELAY) == pdPASS) {
            
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