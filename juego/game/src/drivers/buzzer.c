/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/27
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "buzzer.h"
#include "sapi.h" //especialmente "sapi_sct"
#include "sapi_sct.h"
#include "math.h"

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

#define VOLUME_MAX 255*0.6
#define LENGTH(x) sizeof(x) / sizeof((x)[0])

#define NO_TUNE Sct_SetDutyCycle(BUZZER_PIN, 0) //chequear porq creo q no me funciono

#define T_CORTO 150
#define T_MEDIO T_CORTO*2
#define T_LARGO T_CORTO*3 
 
typedef void (*FuncionMelodia_t)(void);
FuncionMelodia_t playMelodia[] = {soundStartGame, soundDisparoExitoso, soundWinner, soundGameOver, soundReset, soundGame};   


/*
   

*/


//INCHEQUEABLES ESTAS NOTAS :D

Nota_t mario_intro[] = {
  {NOTE_E5, T_CORTO}, {NOTE_E5, T_CORTO}, {SILENCE, T_CORTO}, {NOTE_E5, T_CORTO},
  {SILENCE, T_CORTO}, {NOTE_C5, T_CORTO}, {NOTE_E5, T_CORTO}, {SILENCE, T_CORTO},
  {NOTE_G5, T_CORTO}, {SILENCE, T_LARGO},
};



Nota_t melody_winner[] = {
    {NOTE_G4, 150}, {NOTE_C5, 150}, {NOTE_E5, 150},
    {NOTE_G5, 250}, {SILENCE, 80},
    {NOTE_E5, 150}, {NOTE_G5, 150}, {NOTE_C6, 400},
};

Nota_t melody_gameoverSAD[] = { //prefiero esta
    {NOTE_D5, 200}, {NOTE_C5, 250}, {NOTE_A4, 300},
    {SILENCE, 80},
    {NOTE_G4, 300}, {NOTE_E4, 400}, {NOTE_C4, 500}
};

Nota_t melody_powerup[] = { //GAME OVER HAPPY
   
    {NOTE_D5, 100}, {NOTE_E5, 100}, {NOTE_FS5, 150},
    {NOTE_G5, 150}, {NOTE_A5, 200},
    {SILENCE, 100},
    {NOTE_B5, 150}, {NOTE_D6, 200}, {NOTE_G5, 400}
};

Nota_t melody_gameover[] = { //opcion3
    {NOTE_A4, 200}, {NOTE_G4, 200}, {NOTE_E4, 250},
    {SILENCE, 80},
    {NOTE_D4, 300}, {NOTE_C4, 400}, {NOTE_A3, 500}
};

Nota_t melody_reset[] = {
    {NOTE_G4, 120}, {NOTE_B4, 120}, {NOTE_D5, 150},
    {SILENCE, 80},
    {NOTE_G5, 250}, {NOTE_D5, 200}
};


Nota_t melody1[] = {
    {NOTE_C4, 150}, {NOTE_D4, 150}, {NOTE_G4, 250},
    {SILENCE, 100},
    {NOTE_C5, 250}
};


Nota_t melody2[] = {
    {NOTE_G4, 120}, {NOTE_B4, 120}, {NOTE_D5, 150},
    {SILENCE, 80},
    {NOTE_G5, 250}, {NOTE_D5, 200}
};

Nota_t melody3[] = {
    {NOTE_A4, 200}, {NOTE_G4, 200}, {NOTE_E4, 250},
    {SILENCE, 80},
    {NOTE_D4, 300}, {NOTE_C4, 400}, {NOTE_A3, 500}
};

Nota_t melody4[] = {
    {NOTE_G4, 100}, {NOTE_FS4, 100}, {NOTE_F4, 100}, {NOTE_E4, 150},
    {SILENCE, 60},
    {NOTE_C4, 300}, {NOTE_G3, 400}
};




void buzzerSetFrequency(uint32_t freq){
   Sct_Init(freq);
}

void buzzerStart(uint8_t freq ){
   buzzerSetFrequency(freq);
   pwmConfig(BUZZER_PIN, PWM_ENABLE_OUTPUT);
   //buzzerConfig(BUZZER_PIN, PWM_ENABLE, freq);
}

void buzzerEnd(){
   pwmConfig(BUZZER_PIN, PWM_DISABLE_OUTPUT);
}

void tone(uint32_t nota, int duracion){
   Sct_Init(nota);
   Sct_EnablePwmFor(BUZZER_PIN);
   Sct_SetDutyCycle(BUZZER_PIN, 20);
   #ifdef FREERTOS_CONFIG_H
      vTaskDelay(duracion);
   #else 
      delay(duracion);
   #endif
   NO_TUNE;
}

void play(Melodia_t melodia, int length) {
    for (int i = 0; i < length; i++) {
        if (melodia[i].nota == SILENCE) {
            #ifdef FREERTOS_CONFIG_H
               vTaskDelay(melodia[i].duracion*0.9);
            #else 
               delay(melodia[i].duracion*0.9);
            #endif
        } else {
            tone(melodia[i].nota, melodia[i].duracion);
            //delay(melodia[i].duracion);
        }
    }
}

void soundStartGame(){
   play(mario_intro, sizeof(mario_intro) / sizeof(mario_intro[0]));
}

void soundDisparoExitoso_Basico() {
   gpioWrite(LED1,ON);
    for (int f = 1000; f > 500; f -= 70) {
        tone(f, 10);
        delay(10);
    }
}

void soundDisparoExitoso_Centro() {
    tone(1000, 60);
    delay(70);
    tone(1500, 80);
    delay(100);
    tone(2000, 100);
}

void soundDisparoExitoso(){
   /*
      switch(state){
         case BASICO: soundDisparoExitoso_Basico(); break;
         case CENTRO: soundDisparoExitoso_Centro(); break;
      }
   */
}

void soundWinner(){
   play(melody_winner, sizeof(melody_winner) / sizeof(melody_winner[0]));
}


void soundGameOver(){
   //play(melody_powerup, sizeof(melody_powerup) / sizeof(melody_powerup[0]));
   
   //play(melody_gameoverSAD, sizeof(melody_gameoverSAD) / sizeof(melody_gameoverSAD[0]));
   
   play(melody_gameover, sizeof(melody_gameover) / sizeof(melody_gameover[0]));
   
}

void soundReset(){
    play(melody_reset, sizeof(melody_reset) / sizeof(melody_reset[0]));
}

void soundGame(){

}

