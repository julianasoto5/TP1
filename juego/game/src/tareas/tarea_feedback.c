#include "tarea_juego.h" // Necesario para GameEvent_t y tipos de cola
#include "buzzer.h"
#include "leds.h"
#include "userTasks.h"
#include <stdio.h>
#include "displayTM1637.h"
extern QueueHandle_t xColaFeedback; 
TM1637Display display; // <-- Importante para acceder al objeto

// Define el bitmask del colon
// TÍPICAMENTE es el bit 6: 0x40 (0b01000000). Ajústalo si no funciona.
#define COLON_BITMASK 0b01000000
// Funciones de Control de Drivers 

static void EncenderSecuenciaLEDsInicio(void){
   // printf("[FEEDBACK] -> Secuencia LEDs INICIO (Drivers)\r\n");
   for(int i=0; i<5; i++){
      gpioWrite(LED3, ON);
      vTaskDelay(pdMS_TO_TICKS(20));
      gpioWrite(LEDR, OFF);
   }
}

static void ReproducirSonidoInicio(void){
    //printf("[FEEDBACK] -> Sonido INICIO (Drivers)\r\n");
   soundStartGame();
}

static void EncenderLEDsVerdes(void){
    //printf("[FEEDBACK] -> LEDs VERDES (Drivers)\r\n");
   for(int i=0; i<2; i++){
      gpioWrite(LED3, ON);
      vTaskDelay(pdMS_TO_TICKS(10));
      gpioWrite(LEDR, OFF);
   }
}

static void ReproducirSonidoAcierto(uint8_t estado){
   //printf("[FEEDBACK] -> Sonido ACIERTO (Drivers)\r\n");
   //playMelodia[DISPARO_EXITOSO]()(estado);
   if(estado)
      soundDisparoExitoso_Centro();
   else soundDisparoExitoso_Basico();
}


static void MostrarTiempoLCD(uint16_t tiempo_restante, TM1637Display display){
   
    
   tm1637_show_number_dec_ex(&display, (int)tiempo_restante, dotsON, true, 4, 0);
}

static void EncenderSecuenciaLEDsFinal(void){
   // printf("[FEEDBACK] -> Secuencia LEDs FINAL (Drivers)\r\n");
}

static void ReproducirSonidoFinal(void){
    //printf("[FEEDBACK] -> Sonido FINAL (Drivers)\r\n");
   soundWinner();
}

/* Tarea principal */

void tarea_feedback(void *pvParameters)
{
    GameEvent_t evento;
   tm1637_init(&display, CLK, DIO, 5); // pinCLK=GPIO5, pinDIO=GPIO7
   tm1637_set_brightness(&display, 7, true);
   tm1637_show_number_dec_ex(&display, MAX_TIEMPO_PARTIDA, dotsON, true, 4, 0);
   lucesInit();
   lucesIDLE();
    while (1) {
      /*gpioWrite(LED1, OFF);
      gpioWrite(LED2, OFF);
      gpioWrite(LED3, ON);
      gpioWrite(LEDR, OFF);
      gpioWrite(LEDB, OFF);*/
     
        // Loop: Esperar evento desde la tarea de Juego 
        if (xQueueReceive(xColaFeedback, &evento, 0) == pdPASS) {
            
            if (evento.tipo == START_GAME) {
                ReproducirSonidoInicio(); 
            }
            else if (evento.tipo == FIN_PARTIDA) {
                lucesFIN_PARTIDO();
                ReproducirSonidoFinal();
                MostrarTiempoLCD(0, display); //se podria mostrar el puntaje
                lucesIDLE();
               
            }
            else if(evento.tipo == GAME_RESET){
               MostrarTiempoLCD(0, display);
               lucesIDLE();
            }
            else if (evento.tipo == TICK_10MS) {
                MostrarTiempoLCD(evento.valor, display); // La tarea juego envía el tiempo en valor
            }
           else if (evento.tipo == DISPARO) {
                lucesDISPARO();        
                ReproducirSonidoAcierto(evento.valor);     
            }
        }else if ((evento.tipo != GAME_RESET)&&(evento.tipo != FIN_PARTIDA)) //playing
               lucesPLAYING();
           
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}