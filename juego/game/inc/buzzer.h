/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/27
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __BUZZER_H__
#define __BUZZER_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include <stdint.h>
#include <stddef.h>

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif



// Notas musicales (frecuencias en Hz)
#define NOTE_C3   131
#define NOTE_CS3  139
#define NOTE_D3   147
#define NOTE_DS3  156
#define NOTE_E3   165
#define NOTE_F3   175
#define NOTE_FS3  185
#define NOTE_G3   196
#define NOTE_GS3  208
#define NOTE_A3   220
#define NOTE_AS3  233
#define NOTE_B3   247

#define NOTE_C4   262
#define NOTE_CS4  277
#define NOTE_D4   294
#define NOTE_DS4  311
#define NOTE_E4   330
#define NOTE_F4   349
#define NOTE_FS4  370
#define NOTE_G4   392
#define NOTE_GS4  415
#define NOTE_A4   440
#define NOTE_AS4  466
#define NOTE_B4   494

#define NOTE_C5   523
#define NOTE_CS5  554
#define NOTE_D5   587
#define NOTE_DS5  622
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_FS5  740
#define NOTE_G5   784
#define NOTE_GS5  831
#define NOTE_A5   880
#define NOTE_AS5  932
#define NOTE_B5   988

#define NOTE_C6   1047
#define NOTE_CS6  1109
#define NOTE_D6   1175
#define NOTE_DS6  1245
#define NOTE_E6   1319
#define NOTE_F6   1397
#define NOTE_FS6  1480
#define NOTE_G6   1568
#define NOTE_GS6  1661
#define NOTE_A6   1760
#define NOTE_AS6  1865
#define NOTE_B6   1976

#define NOTE_C7   2093
#define NOTE_CS7  2217
#define NOTE_D7   2349
#define NOTE_DS7  2489
#define NOTE_E7   2637
#define NOTE_F7   2794
#define NOTE_FS7  2960
#define NOTE_G7   3136
#define NOTE_GS7  3322
#define NOTE_A7   3520
#define NOTE_AS7  3729
#define NOTE_B7   3951

#define SILENCE   0



#define BUZZER_PIN CTOUT8//SPI_MISO
#define CANT_SONIDOS 5

//estructura que tenga nota + duracion?
typedef struct{
   uint16_t nota;
   uint16_t duracion;
} Nota_t;

typedef Nota_t* Melodia_t;

typedef enum {FEEDBACK_START_GAME, DISPARO_EXITOSO, WINNER, FEEDBACK_GAME_OVER, SOUND_RESET, GAME} FeedbackState_t;
typedef enum {BASICO, CENTRO} estado; //LO QUE SE RECIBIRIA DE LA COLA 

void soundStartGame();
void soundDisparoExitoso();
void soundDisparoExitoso_Centro();
void soundDisparoExitoso_Basico();
void soundWinner();
void soundGameOver();
void soundReset();
void soundGame();

void buzzer_Init();
void setBuzzer(uint8_t note, uint8_t duracion);

#define BUZZER_PERIOD_MS 1


/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __BUZZER_H__ */




/*

MELODIAS DESCARTADAS
void soundDisparoExitoso2() {
    for (int i = 0; i < 60; i++) {
        int freq = 1000 + 300 * sin(i * 0.3);
        tone(freq, 5);
        delay(5);
    }
}

Nota_t melody_victory[] = {
  {NOTE_E4, 200}, {NOTE_G4, 200}, {NOTE_A4, 200},
  {NOTE_B4, 200}, {NOTE_E5, 300},
  {SILENCE, 100},
  {NOTE_E5, 150}, {NOTE_G5, 150}, {NOTE_A5, 150},
  {NOTE_B5, 300}, {NOTE_E6, 400}
};

Nota_t melody_winner2[] = {
    {NOTE_C4, 150}, {NOTE_D4, 150}, {NOTE_E4, 150},
    {NOTE_G4, 200}, {NOTE_C5, 250}, {SILENCE, 100},
    {NOTE_B4, 150}, {NOTE_C5, 200}, {NOTE_E5, 250},
    {NOTE_G5, 300}, {SILENCE, 100},
    {NOTE_C6, 400}
};

Nota_t melody_winner3[] = {
    {NOTE_E4, 150}, {NOTE_G4, 150}, {NOTE_B4, 150},
    {NOTE_E5, 250}, {SILENCE, 100},
    {NOTE_G4, 150}, {NOTE_B4, 150}, {NOTE_D5, 200},
    {NOTE_G5, 300}, {SILENCE, 80},
    {NOTE_C5, 150}, {NOTE_D5, 150}, {NOTE_E5, 200},
    {NOTE_G5, 300}, {NOTE_C6, 500}
};

Nota_t melody_gameover1[] = {
    {NOTE_E5, 200}, {NOTE_D5, 200}, {NOTE_C5, 200},
    {SILENCE, 80},
    {NOTE_G4, 300}, {NOTE_C4, 500}
};

Nota_t melody_gameover2[] = {
    {NOTE_E5, 200}, {NOTE_G4, 150}, {NOTE_E4, 250},
    {SILENCE, 100},
    {NOTE_D4, 150}, {NOTE_C4, 400},
    {SILENCE, 100},
    {NOTE_C4, 600}
};

Nota_t melody_gameover3[] = {
    {NOTE_C5, 150}, {NOTE_B4, 150}, {NOTE_A4, 150},
    {NOTE_G4, 150}, {NOTE_F4, 200},
    {SILENCE, 100},
    {NOTE_E4, 150}, {NOTE_D4, 150}, {NOTE_C4, 400}
};

Nota_t melody_gameover4[] = {
    {NOTE_G5, 200}, {NOTE_E5, 200}, {NOTE_C5, 200},
    {SILENCE, 80},
    {NOTE_F4, 150}, {NOTE_D4, 150}, {NOTE_C4, 400},
    {SILENCE, 120},
    {NOTE_C4, 500}
};

*/
