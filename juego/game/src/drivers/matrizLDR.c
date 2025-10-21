/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/06
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

/*
Tarea que se ejecuta cada x ms. Almacena los anillos golpeados en la variable 
anillos_alcanzados,
*/

#include "matrizLDR.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/
    
uint16_t muestra = 0; //10 bits
uint16_t umbral = UMBRAL_DEFAULT;

uint8_t matrizPinesFilas[CANT_FILAS] = {
                                 DIG_PIN_OUT1,
                                 DIG_PIN_OUT2,
                                 //DIG_PIN_OUT3,
                                // DIG_PIN_OUT4,
                               };


uint8_t matrizPinesColumnas[CANT_COLUMNAS] = {
                                    //AN_PIN_1,
                                    AN_PIN_2,
                                    AN_PIN_3, 
                                    //AN_PIN_4, //---hijo de puta
                                  };
/*=====[Definition macros of public constants]===============================*/
const int anillos[CANT_FILAS][CANT_COLUMNAS] = {{1,1},
                                                {2,2},
                                                //{1,1},
                                                //{1,1},
                                               }; //todavia no está implementado esto
void matrizLDR_Init( void )
{
   
   uint8_t pin = 0;    
   adcConfig(ADC_ENABLE); //habilitacion de columnas analógicas
   for( pin=0; pin<CANT_FILAS; pin++ ){      
      // Configuro las filas como salida
      gpioInit( matrizPinesFilas[pin], GPIO_OUTPUT );
      gpioWrite( matrizPinesFilas[pin], ON );
   }
   
}

int muestreoLDR(){
   int led;
   uint8_t ledMap[CANT_FILAS * CANT_COLUMNAS] = {LED1, LED2, LED3, LEDB};
   int ringHit = 0;
   
   for(int pin=0; pin<CANT_FILAS; pin++){
      gpioWrite(matrizPinesFilas[pin], OFF);
      
      // DELAY CRÍTICO: Espera a que los LDR se estabilicen
    
      
      for (int i=0; i<CANT_COLUMNAS; i++){
         muestra = adcRead( matrizPinesColumnas[i] );
         led = pin + CANT_FILAS * i;
         
         if(muestra < umbral){
            gpioWrite(ledMap[led], ON);
            ringHit = anillos[pin][i]; 
            
            gpioWrite(matrizPinesFilas[pin], ON);
            return ringHit;
         }
         else{
            gpioWrite(ledMap[led], OFF);
            //printf("Valor ADC, LDR %d: %u\r\n", led, muestra);
         }
      }
      
      gpioWrite(matrizPinesFilas[pin], ON);
      
     
   }  
   return 0;
}
uint16_t matrizLDR_Test(int ldr){
   int digitalPin = ldr/CANT_FILAS, analogPin = ldr%CANT_COLUMNAS;
   uint16_t test;
   gpioWrite(digitalPin, OFF);
   test = adcRead(analogPin);
   gpioWrite(digitalPin, ON);
   return test;
}

void matrizLDR_SetUmbral(int testDark, int testLight){
   umbral = SET_UMBRAL(testDark, testLight);
   printf("Umbral seteado a %d\r\n",umbral);
}
