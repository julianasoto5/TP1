/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/15
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __GAME_H__
#define __GAME_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "sapi.h"
#include "matrizLDR.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
#define CANT_ANILLOS 2
#define HIGHEST_SCORE 100
#define BASE 2
#define GET_SCORE(anillo) HIGHEST_SCORE*pow(BASE, anillo-CANT_ANILLOS) //debe dividir
   
/*=====[Prototypes (declarations) of public functions]=======================*/

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __GAME_H__ */
