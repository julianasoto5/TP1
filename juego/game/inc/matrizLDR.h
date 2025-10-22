/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/06
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __TEST_H__
#define __TEST_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include <stdint.h>
#include <stddef.h>
#include "sapi.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#define CANT_FILAS 2
#define CANT_COLUMNAS 2
//Definicion de filas digitales
#define DIG_PIN_OUT1 T_FIL0
#define DIG_PIN_OUT2 T_FIL1
#define DIG_PIN_OUT3 T_FIL2
#define DIG_PIN_OUT4 T_COL0
   
//Definicion de columnas analogicas
#define AN_PIN_1 CH1
#define AN_PIN_2 CH2
#define AN_PIN_3 CH3
#define AN_PIN_4 CH4
   
   
#define UMBRAL_DEFAULT 850
#define SET_UMBRAL(testDark, testLight) (testDark+testLight)/2 

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/


/*=====[Prototypes (declarations) of public functions]=======================*/
void matrizLDR_Init( void );
uint8_t matrizLDR_Scan(uint8_t*);
uint16_t matrizLDR_Test(int);


/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __TEST_H__ */