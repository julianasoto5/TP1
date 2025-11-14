#ifndef __TM1637_H__
#define __TM1637_H__

#include "sapi.h"
 
#define CLK GPIO5
#define DIO GPIO7

typedef struct {
    gpioMap_t pinCLK;
    gpioMap_t pinDIO;
    uint8_t brightness;
    unsigned int bitDelay;
} TM1637Display;

// Funciones publicas //

/**Inicializador del TM1637**/
void tm1637_init(TM1637Display* , gpioMap_t , gpioMap_t , unsigned int );
/**Control de brillo: Ajusta el brillo (0-7) y enciende y apaga el display**/
void tm1637_set_brightness(TM1637Display* , uint8_t , bool );
/**Establecer segmentos: Envía datos crudos de segmentos al display.**/
void tm1637_set_segments(TM1637Display* , const uint8_t [], uint8_t , uint8_t );
/**Limpiar display: Apaga todos los segmentos**/
void tm1637_clear(TM1637Display* );
/**Mostrar número decimal: Convierte un número entero a formato de display.**/
void tm1637_show_number_dec(TM1637Display* , int , bool , uint8_t , uint8_t );
void tm1637_show_number_dec_ex(TM1637Display* , int , uint8_t , bool , uint8_t , uint8_t );
void tm1637_show_number_hex_ex(TM1637Display* , uint16_t , uint8_t , bool , uint8_t , uint8_t );
/**Codear digito: Convierte un valor numérico (0-15) en el patrón de bits correspondiente para encender los segmentos correctos del display de 7 segmentos**/
uint8_t tm1637_encode_digit(uint8_t );


// Funciones privadas //

void tm1637_bit_delay(TM1637Display* );
void tm1637_start(TM1637Display* );
void tm1637_stop(TM1637Display* );
bool tm1637_write_byte(TM1637Display* , uint8_t );
void tm1637_show_dots(TM1637Display* , uint8_t , uint8_t* );
void tm1637_show_number_base_ex(TM1637Display* , int8_t , uint16_t , uint8_t , bool , uint8_t , uint8_t );

#endif /* __TM1637_H__ */
