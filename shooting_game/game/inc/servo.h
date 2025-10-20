/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/17
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __SERVO_H__
#define __SERVO_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include <stdint.h>
#include <stddef.h>


#include "sapi.h"
#include <time.h>
/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#define SERVO1_PIN SERVO4 //GPIO8
#define SERVO2_PIN SERVO8 //GPIO2  
   
#define POS_INICIAL_SERVO1 0 //entre 0 y 180
#define POS_INICIAL_SERVO2 0 //entre 0 y 180
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

#endif /* __SERVO_H__ */
