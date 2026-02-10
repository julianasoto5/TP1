#ifndef __LEDS_H__
#define __LEDS_H__
#endif
#include "sapi.h"
#include "FreeRTOS.h"
#include "Task.h"
#define LED_PIN GPIO3

void lucesInit();
void lucesIDLE();

void lucesPLAYING();
void lucesDISPARO();
void lucesFIN_PARTIDO();
