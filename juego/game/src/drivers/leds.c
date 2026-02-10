#include "leds.h"
void lucesInit(){
   gpioConfig(LED_PIN, OUTPUT);
   gpioWrite(LED_PIN, OFF);
}


void lucesIDLE(){
   gpioWrite(LED_PIN, ON);
}

void lucesPLAYING(){
   gpioToggle(LED_PIN);
   vTaskDelay(pdMS_TO_TICKS(100));
}

void lucesDISPARO(){
   gpioToggle(LED_PIN);
   vTaskDelay(pdMS_TO_TICKS(50));
   gpioToggle(LED_PIN);
   vTaskDelay(pdMS_TO_TICKS(50));
   gpioToggle(LED_PIN);
   vTaskDelay(pdMS_TO_TICKS(50));
   gpioToggle(LED_PIN);
}


void lucesFIN_PARTIDO(){
    gpioToggle(LED_PIN);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpioToggle(LED_PIN);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpioToggle(LED_PIN);
   
   }
