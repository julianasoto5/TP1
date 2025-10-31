/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/27
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "buzzer.h"
#include "sapi.h" //especialmente "sapi_sct"
#include "sapi_sct.h"

/*
   No es posible usar la librería "sapi_pwm" porque la misma solo permite
   modificar el ciclo de trabajo, y en este caso es necesario cambiar la 
   frecuencia de la señal. En consecuencia, la señal PWM se realizó 
   manipulando directamente el SCT (State Configurable Timer) a partir de 
   la libreria "sapi_sct".
   
   Sin embargo, esta libreria solo trabaja de a una frecuencia a la vez 
   (configurada al hacer el Init), lo que genera una incompatibilidad 
   de uso con la funcionalidad de este driver. El buzzer debe poder 
   generar sonidos a distinta frecuencia, por lo que la solucion propuesta
   es hacer la inicializacion del SCT cada vez que se quiera cambiar de 
   nota. En un futuro, se puede implementar usando la funcion que utiliza 
   la misma libreria "sapi_sct" para configurar la frecuencia:
   Chip_SCTPWM_SetRate(LPC_SCT, frequency);
   
   Frecuencia -- tipo de sonido
   Duty cycle -- volumen de sonido
      --volumen maximo con DC = 50%
   

*/
/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

#define VOLUME_MAX 128
#define LENGTH(x) sizeof(x) / sizeof((x)[0])

//INCHEQUEABLES ESTAS NOTAS :D

// inicio de Mario
Nota_t game[] = {{MI5, 150}, {MI5, 150}, {0, 150}, {MI5, 150}, 
                {0, 150}, {DO5, 150}, {MI5, 150}, {0, 150},
                {SOL5, 150}, {0, 150}, {0, 150}, {0, 150}, 
                {SOL4, 150}, {0, 150}, {0, 150}, {0, 150},
               };

//We are the champions
Nota_t winner[] = {
        {FA4, 400},  {SOL4,200},  {FA4,200},   {MI4,200},
        {FA4, 200},  {SOL4, 200}, {LA4, 400},  {SOL4, 200},
        {FA4, 200},  {MI4, 200},  {RE4, 200},  {DO4, 400},
        {RE4, 200},  {MI4, 200},  {FA4, 400},  {0,0},
    };               
               
//Taps -- no se como se supone que tiene que sonar
Nota_t game_over[] = {
         {DO4, 500},  {DO4, 500},  {DO4, 500}, {SOL4, 500}, 
         {DO5, 500},  {SOL4, 500}, {DO5, 500}, {DO4, 500},
         {SOL4, 500}, {DO4, 500},  {DO4, 500}, {SOL4, 500}, 
         {FA4, 500},  {MI4, 500},  {RE4, 500}, {DO4, 1000},
      };
      
Nota_t reset[] = {
         {MI5, 80},  {SOL5, 80},  {DO6, 120}, {0, 40}, 
         {DO6, 80},  {SOL5, 70}, {MI5, 120},
      };

//Marcha imperial      
Nota_t success[] = { //creo que esta va a quedar muy larga igual
         {LA4, 500},  {LA4, 500},  {LA4, 500}, {FA4, 350}, 
         {DO5, 150},  {LA4, 500}, {FA4, 350}, {DO5, 150},
         {LA4, 650}, 
      };

Melodia_t melodias[CANT_SONIDOS] = {    //si no funciona el LENGTH cambiar por la cantidad de notas
                                    {success, LENGTH(success)}, 
                                    {winner, LENGTH(winner)}, 
                                    {game_over, LENGTH(winner)}, 
                                    {reset, LENGTH(reset)}, 
                                    {game, LENGTH(game)} 
                                    };

//typedef enum {DISPARO_EXITOSO, WINNER, GAME_OVER, FEEDBACK_RESET, GAME} FeedbackState_t;
void buzzerSetFrequency(uint32_t freq){
   Sct_Init(freq);
   
}

void buzzerStart(uint8_t freq ){
   buzzerSetFrequency(freq);
   pwmConfig(BUZZER_PIN, PWM_ENABLE_OUTPUT);
   //buzzerConfig(BUZZER_PIN, PWM_ENABLE, freq);
}

void buzzerEnd(){
   //pwmConfig(BUZZER_PIN, PWM_);
}


void play(Melodia_t m){
   //pwmWrite(PIN, DUTY_CICLE);
   
   for(int i=0; i<m.cant_notas; i++){
      buzzerStart(m.melodia[i].nota);
      pwmWrite(BUZZER_PIN, VOLUME_MAX);
      #ifdef FREERTOS_CONFIG_H
         vTaskDelay(m.melodia[i].duracion);
      #else 
         delay(m.melodia[i].duracion);
      #endif
      //buzzerEnd();
   }
   
}


void buzzerPlaySound(FeedbackState_t type){
   play(melodias[type]);   
}


int main( void )
{
   // ----- Setup -----------------------------------
   boardInit();
   
   // ----- Repeat for ever -------------------------
   while( true ) {
      gpioToggle(LED);
      play(melodias[GAME]);
      delay(500);
   }

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}
