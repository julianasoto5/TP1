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
uint16_t umbral = UMBRAL_DEFAULT;

void matrizLDR_Init( void )
{
   uartConfig(UART_USB, 9600);   // Inicializa UART por USB a 9600 baudios
   uint8_t pin = 0;    
   adcConfig(ADC_ENABLE); //habilitacion de columnas anal�gicas
   for( pin=0; pin<CANT_FILAS; pin++ ){      
      // Configuro las filas como salida
      gpioInit( matrizPinesFilas[pin], GPIO_OUTPUT );
      gpioWrite( matrizPinesFilas[pin], ON );
   }
   
}

void muestreoLDR(){
   int led;
   uint8_t ledMap[CANT_FILAS * CANT_COLUMNAS] = {LED1, LED2, LED3, LEDB};//, LEDR, LEDG}; 
   
   int ringHit;
   // ----- Repeat for ever -------------------------
   
   for(int pin=0; pin<CANT_FILAS; pin++){
      gpioWrite(matrizPinesFilas[pin], OFF);
      for (int i=0; i<CANT_COLUMNAS; i++){
         muestra = adcRead( matrizPinesColumnas[i] );
         led = pin+CANT_FILAS*i; 
         if(muestra < umbral){
            gpioWrite(ledMap[led], ON);
            //if((led==3)) gpioWrite(LEDR,ON);
            ringHit = anillos[i][pin]; //ESTE VALOR ES EL QUE TENGO QUE COMUNICAR A LA TAREA JUEGO
            printf("Valor ADC, LDR %d: %u. Anillo: %d\r\n", led,muestra, ringHit);
            //interrumpir - devolver valor?
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


/*No s� bien c�mo integrarlo con el comando desde la APP, porque si uso solo el Config, 
  necesitar�a que el usuario notifique en el medio de la funci�n cu�ndo termina de testear.
  Capaz es mejor dejar al funci�n Test y que desde game.c se maneje para que aparte se pueda 
  testear Luz/Oscuridad en el orden que el usuario quiera.

*/
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




/* // ... [Inclusions and definitions] ...

// Modificar la firma de la función para que retorne un valor
int muestreoLDR(){ // Retornará el ring impactado (0-4) o -1 si no hay impacto
   int led;
   uint8_t ledMap[CANT_FILAS * CANT_COLUMNAS] = {LED1, LED2, LED3, LEDB};//, LEDR, LEDG}; 
   
   int ringHit = -1; // Inicializamos a -1 (No hit)
   // ----- Repeat for ever -------------------------
   
   for(int pin=0; pin<CANT_FILAS; pin++){
      gpioWrite(matrizPinesFilas[pin], OFF);
      for (int i=0; i<CANT_COLUMNAS; i++){
         muestra = adcRead( matrizPinesColumnas[i] );
         led = pin+CANT_FILAS*i; 
         if(muestra < umbral){
            gpioWrite(ledMap[led], ON);
            //if((led==3)) gpioWrite(LEDR,ON);
            ringHit = anillos[i][pin]; // Guardamos el anillo impactado
            printf("Valor ADC, LDR %d: %u. Anillo: %d\r\n", led,muestra, ringHit);
             
             // *** IMPORTANTE: Si se detecta un hit, salimos del muestreo para informar INMEDIATAMENTE ***
             // y no seguir muestreando el resto de la matriz, priorizando la respuesta en tiempo real.
             gpioWrite(matrizPinesFilas[pin], ON); // Restaurar el pin antes de salir
             return ringHit; 

         }
         else{
            gpioWrite(ledMap[led], OFF);
            //if((led==3)) gpioWrite(LEDR,OFF);
            printf("---------------------------------Valor ADC, LDR %d: %u\r\n", led,muestra);
         }
      }
      gpioWrite(matrizPinesFilas[pin], ON); // Restaurar la fila
      delay(50); // Puedes ajustar este delay si el muestreo completo es muy lento

   }     
    return -1; // Si el bucle termina sin un hit, devuelve -1
}

// ... [Resto de las funciones] ...

*/


/*
int main( void )
{
   // ----- Setup -----------------------------------
   boardInit();
   
   
   //Configuraci?n de estado inicial del Led 
   //bool_t ledState1 = OFF;
   //gpioMap_t led = (gpioMap_t)ptr;
   matrizLDR_Init();
   

   int led;
   int ringHit;
   uint8_t ledMap[CANT_FILAS * CANT_COLUMNAS] = {LED1, LED2, LED3, LEDB};//, LEDR, LEDG}; 
   // ----- Repeat for ever -------------------------
   while( true ) {
      for(int pin=0; pin<CANT_FILAS; pin++){
         gpioWrite(matrizPinesFilas[pin], OFF);
         for (int i=0; i<CANT_COLUMNAS; i++){
            muestra = adcRead( matrizPinesColumnas[i] );
            led = pin+CANT_FILAS*i; 
            if(muestra < umbral){
               gpioWrite(ledMap[led], ON);
               //if((led==3)) gpioWrite(LEDR,ON);
               ringHit = anillos[i][pin]; //ESTE VALOR ES EL QUE TENGO QUE COMUNICAR A LA TAREA JUEGO
               printf("Valor ADC, LDR %d: %u. Anillo: %d\r\n", led,muestra, ringHit);
               
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
}*/