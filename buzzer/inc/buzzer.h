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
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_E5  659
#define NOTE_G5  784
   
   
#define DO4  262
#define RE4  294
#define MI4  330
#define FA4  349
#define SOL4 392
#define LA4  440
#define SI4  494
#define DO5  523
#define RE5  587
#define MI5  659
#define FA5  698
#define SOL5 784
#define LA5  880
#define SI5  988
#define DO6  1047

typedef enum {DISPARO_EXITOSO, WINNER, GAME_OVER, FEEDBACK_RESET, GAME} FeedbackState_t;




#define BUZZER_PIN PWM4//T_FIL3 
#define CANT_NOTAS 16
#define CANT_SONIDOS 5

//estructura que tenga nota + duracion?
typedef struct{
   uint16_t nota;
   uint16_t duracion;
} Nota_t;

typedef struct{
   Nota_t* melodia;
   uint8_t cant_notas;
} Melodia_t;




void buzzerInit();
void playSound(FeedbackState_t);
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
