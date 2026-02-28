#include "tarea_juego.h" // Necesario para GameEvent_t y tipos de cola
#include "buzzer.h"
#include "leds.h"
#include "userTasks.h"
#include <stdio.h>
#include "displayTM1637.h"
extern QueueHandle_t xColaFeedback; 
TM1637Display display; 


static void ReproducirSonidoInicio(void){
   
   soundStartGame();
}


static void ReproducirSonidoAcierto(uint8_t estado){
   if(estado)
      soundDisparoExitoso_Centro();
   else soundDisparoExitoso_Basico();
}


static void MostrarTiempoLCD(uint16_t tiempo_restante, TM1637Display display){
   
    
   tm1637_show_number_dec_ex(&display, (int)tiempo_restante, dotsON, true, 4, 0);
}



static void ReproducirSonidoFinal(void){
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
      
     
        // Loop: Esperar evento desde la tarea de Juego 
        if (xQueueReceive(xColaFeedback, &evento, pdMS_TO_TICKS(1)) == pdPASS) {
            
            if (evento.tipo == START_GAME) {
                ReproducirSonidoInicio();
            }
            else if (evento.tipo == FIN_PARTIDA) {
                lucesFIN_PARTIDO();
                ReproducirSonidoFinal();
                MostrarTiempoLCD(0, display); //se podria mostrar el puntaje
                lucesIDLE();
               xQueueReset(xColaFeedback);
            }
            else if(evento.tipo == GAME_RESET){
               xQueueReset(xColaFeedback);
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
           
        //vTaskDelay(pdMS_TO_TICKS(2));
    }
}