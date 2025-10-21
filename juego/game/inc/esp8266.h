/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/13
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __ESP_H__
#define __ESP_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include <stdint.h>
#include <stddef.h>

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "sapi.h"   
#include "string.h" 
#include <stdlib.h> 
#include <stdio.h>  

#define ESP_UART_PORT   UART_232 
#define UART_BAUD_RATE  115200 
#define IP_BUFFER_SIZE  16 // Suficiente para "xxx.xxx.xxx.xxx"
/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
typedef struct {
    int score;
    int time_left;
    char state[15]; 
    int level;
} GameState_t;

/*=====[Prototypes (declarations) of public functions]=======================*/
void sendStatusToESP(void);
void checkCommandsFromESP(void);

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __ESP_H__ */
