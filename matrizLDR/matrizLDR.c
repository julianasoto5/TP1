/*=============================================================================
 * Author: JS < >
 * Date: 2025/10/06
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "matrizLDR.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/
    
uint16_t muestra = 0; //10 bits


void matrizLDR_Init( void )
{
   uartConfig(UART_USB, 9600);   // Inicializa UART por USB a 9600 baudios
   uint8_t pin = 0;    
   adcConfig(ADC_ENABLE); //habilitacion de columnas analógicas
   for( pin=0; pin<CANT_FILAS; pin++ ){      
      // Configuro las filas como salida
      gpioInit( matrizPinesFilas[pin], GPIO_OUTPUT );
      gpioWrite( matrizPinesFilas[pin], ON );
   }
   
}

void muestroLDR(){
   int led;
   uint8_t ledMap[CANT_FILAS * CANT_COLUMNAS] = {LED1, LED2, LED3, LEDB};//, LEDR, LEDG}; 
   // ----- Repeat for ever -------------------------
   while( true ) {
      for(int pin=0; pin<CANT_FILAS; pin++){
         gpioWrite(matrizPinesFilas[pin], OFF);
         for (int i=0; i<CANT_COLUMNAS; i++){
            muestra = adcRead( matrizPinesColumnas[i] );
            led = pin+CANT_FILAS*i; 
            if(muestra < 850){
               gpioWrite(ledMap[led], ON);
               //if((led==3)) gpioWrite(LEDR,ON);
               printf("Valor ADC, LDR %d: %u\r\n", led,muestra);
               //interrumpipr?
            }
            else{
               gpioWrite(ledMap[led], OFF);
               //if((led==3)) gpioWrite(LEDR,OFF);
               printf("---------------------------------Valor ADC, LDR %d: %u\r\n", led,muestra);
            }
         }
         gpioWrite(matrizPinesFilas[pin], ON);
         delay(50);
            
      }     }
   }


int main( void )
{
   // ----- Setup -----------------------------------
   boardInit();
   
   
   //Configuración de estado inicial del Led 
   //bool_t ledState1 = OFF;
   //gpioMap_t led = (gpioMap_t)ptr;
   matrizLDR_Init();
   

   int led;
   uint8_t ledMap[CANT_FILAS * CANT_COLUMNAS] = {LED1, LED2, LED3, LEDB};//, LEDR, LEDG}; 
   // ----- Repeat for ever -------------------------
   while( true ) {
      for(int pin=0; pin<CANT_FILAS; pin++){
         gpioWrite(matrizPinesFilas[pin], OFF);
         for (int i=0; i<CANT_COLUMNAS; i++){
            muestra = adcRead( matrizPinesColumnas[i] );
            led = pin+CANT_FILAS*i; 
            if(muestra < 850){
               gpioWrite(ledMap[led], ON);
               //if((led==3)) gpioWrite(LEDR,ON);
               printf("Valor ADC, LDR %d: %u\r\n", led,muestra);
               //interrumpipr?
            }
            else{
               gpioWrite(ledMap[led], OFF);
               //if((led==3)) gpioWrite(LEDR,OFF);
               printf("---------------------------------Valor ADC, LDR %d: %u\r\n", led,muestra);
            }
         }
         gpioWrite(matrizPinesFilas[pin], ON);
         delay(50);
            
      }
         
   }

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}
