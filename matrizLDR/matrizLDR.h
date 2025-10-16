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
   
//Definicion de columnas analogicas
#define AN_PIN_1 ADC_CH2
#define AN_PIN_2 DAC

/*=====[Definition macros of public constants]===============================*/
const int anillos[CANT_FILAS][CANT_COLUMNAS] = {{1,1},
                                                {2,2},
                                               }; //todavia no está implementado esto

uint8_t matrizPinesFilas[CANT_FILAS] = {
                                 DIG_PIN_OUT1,
                                 DIG_PIN_OUT2,
                                // DIG_PIN_OUT3,
                               };


uint8_t matrizPinesColumnas[CANT_COLUMNAS] = {
                                    AN_PIN_1,
                                    AN_PIN_2,
                                  };
/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/


/*=====[Prototypes (declarations) of public functions]=======================*/
void matrizLDR_Init( void );

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __TEST_H__ */
