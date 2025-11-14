#include "TM1637.h"

// Comandos del TM1637
#define TM1637_I2C_COMM1    0x40
#define TM1637_I2C_COMM2    0xC0
#define TM1637_I2C_COMM3    0x80
#define dotsON 0b01000000

/** Mapeo dígito a segmentos:
Cada byte representa qué segmentos encender para cada dígito (0-F).
      A
     ---
  F |   | B
     -G-
  E |   | C
     ---
      D
**/
const uint8_t digitToSegment[] = {
 // XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
  0b01110111,    // A
  0b01111100,    // B
  0b00111001,    // C
  0b01011110,    // D
  0b01111001,    // E
  0b01110001     // F
  };

// === FUNCIONES DE BAJO NIVEL ===

void tm1637_bit_delay(TM1637Display* display) {
    delayInaccurateUs(display->bitDelay);
}

void tm1637_start(TM1637Display* display) {
    gpioConfig(display->pinDIO, GPIO_OUTPUT);
    tm1637_bit_delay(display);
}

void tm1637_stop(TM1637Display* display) {
    gpioConfig(display->pinDIO, GPIO_OUTPUT);
    tm1637_bit_delay(display);
    gpioConfig(display->pinCLK, GPIO_INPUT);
    tm1637_bit_delay(display);
    gpioConfig(display->pinDIO, GPIO_INPUT);
    tm1637_bit_delay(display);
}

bool tm1637_write_byte(TM1637Display* display, uint8_t b) {
    uint8_t data = b;

    // Enviar 8 bits
    for(uint8_t i = 0; i < 8; i++) {
        // pinCLK bajo
        gpioConfig(display->pinCLK, GPIO_OUTPUT);
        tm1637_bit_delay(display);

        // Establecer bit de datos
        if (data & 0x01) {
            gpioConfig(display->pinDIO, GPIO_INPUT); // HIGH (pull-up)
        } else {
            gpioConfig(display->pinDIO, GPIO_OUTPUT);
            gpioWrite(display->pinDIO, LOW);
        }

        tm1637_bit_delay(display);

        // pinCLK alto
        gpioConfig(display->pinCLK, GPIO_INPUT);
        tm1637_bit_delay(display);
        data = data >> 1;
    }
   // Esperar ACK
    gpioConfig(display->pinCLK, GPIO_OUTPUT);
    gpioConfig(display->pinDIO, GPIO_INPUT);
    tm1637_bit_delay(display);

    gpioConfig(display->pinCLK, GPIO_INPUT);
    tm1637_bit_delay(display);
    
    uint8_t ack = gpioRead(display->pinDIO);
    if (ack == 0) {
        gpioConfig(display->pinDIO, GPIO_OUTPUT);
    }

    tm1637_bit_delay(display);
    gpioConfig(display->pinCLK, GPIO_OUTPUT);
    tm1637_bit_delay(display);

    return ack;
}


// === FUNCIONES DE ALTO NIVEL ===

void tm1637_init(TM1637Display* display, gpioMap_t pinCLK, gpioMap_t pinDIO, unsigned int bitDelay) {
    display->pinCLK = pinCLK;
    display->pinDIO = pinDIO;
    display->bitDelay = bitDelay;
    display->brightness = 7; // Brillo máximo por defecto
    
    gpioConfig(pinCLK, GPIO_INPUT);
    gpioConfig(pinDIO, GPIO_INPUT);
    gpioWrite(pinCLK, LOW);
    gpioWrite(pinDIO, LOW);
}

void tm1637_set_brightness(TM1637Display* display, uint8_t brightness, bool on) {
    display->brightness = (brightness & 0x7) | (on ? 0x08 : 0x00);
}

void tm1637_set_segments(TM1637Display* display, const uint8_t segments[], uint8_t length, uint8_t pos) {
    // Escribir COMM1
    tm1637_start(display);
    tm1637_write_byte(display, TM1637_I2C_COMM1);
    tm1637_stop(display);

    // Escribir COMM2 + dirección
    tm1637_start(display);
    tm1637_write_byte(display, TM1637_I2C_COMM2 + (pos & 0x03));

    // Escribir bytes de datos
    for (uint8_t i = 0; i < length; i++) {
        tm1637_write_byte(display, segments[i]);
    }

    tm1637_stop(display);

    // Escribir COMM3 + brillo
    tm1637_start(display);
    tm1637_write_byte(display, TM1637_I2C_COMM3 + (display->brightness & 0x0f));
    tm1637_stop(display);
}

void tm1637_clear(TM1637Display* display) {
    uint8_t data[] = { 0, 0, 0, 0 };
    tm1637_set_segments(display, data, 4, 0);
}

uint8_t tm1637_encode_digit(uint8_t digit) {
    return digitToSegment[digit & 0x0f];
}

void tm1637_show_dots(TM1637Display* display, uint8_t dots, uint8_t* digits) {
    for(int i = 0; i < 4; i++) {
        digits[i] |= (dots & 0x80);
        dots <<= 1;
    }
}

void tm1637_show_number_base_ex(TM1637Display* display, int8_t base, uint16_t num, uint8_t dots, bool leading_zero, uint8_t length, uint8_t pos) {
 
    uint8_t digits[4] = {0};

    if (num == 0 && !leading_zero) {
        // Caso especial para cero sin leading zero
        for(uint8_t i = 0; i < (length-1); i++) {
            digits[i] = 0;
        }
        digits[length-1] = tm1637_encode_digit(0);
    } else {
        // Convertir número a dígitos
        for(int i = length-1; i >= 0; i--) {
            uint8_t digit = num % base;
            
            if (digit == 0 && num == 0 && !leading_zero) {
                // Leading zero en blanco
                digits[i] = 0;
            } else {
                digits[i] = tm1637_encode_digit(digit);
            }
            num /= base;
        }
    }
    
    if(dots != 0) {
        tm1637_show_dots(display, dots, digits);
    }
    
    tm1637_set_segments(display, digits, length, pos);
}

void tm1637_show_number_dec_ex(TM1637Display* display, int num, uint8_t dots, bool leading_zero, uint8_t length, uint8_t pos) {
    tm1637_show_number_base_ex(display, 10, num, dots, leading_zero, length, pos);
}

void tm1637_show_number_hex_ex(TM1637Display* display, uint16_t num, uint8_t dots, bool leading_zero, uint8_t length, uint8_t pos) {
    tm1637_show_number_base_ex(display, 16, num, dots, leading_zero, length, pos);
}

void tm1637_show_countdown (TM1637Display* display, uint16_t seconds) {
    uint8_t minutes = seconds / 60;
    uint8_t secs = seconds % 60;
    uint8_t num = minutes*100 + secs;
    tm1637_show_number_dec_ex(display, num, dotsON, true, 4, 0);
    
    // Cambiar brillo según el tiempo restante
    if (seconds <= 10) {
        // Últimos 10 segundos - parpadeo y brillo bajo
        static bool blink = false;
        if (blink) {
            tm1637_set_brightness(display, 7, true);
        } else {
            tm1637_set_brightness(display, 2, true);
        }
        blink = !blink;
    } else if (seconds <= 30) {
        // Últimos 30 segundos - brillo medio
        tm1637_set_brightness(display, 4, true);
    } else {
        // Tiempo normal - brillo alto
        tm1637_set_brightness(display, 7, true);
    }
}


int main( void )
{  
   // ----- Setup -----------
   boardInit();
   
   // Inicializar display (ajusta los pines según tu conexión)
   TM1637Display display;
   tm1637_init(&display, CLK, DIO, 100); // pinCLK=GPIO5, pinDIO=GPIO7
   tm1637_set_brightness(&display, 7, true);
   tm1637_show_number_dec_ex(&display, 0, dotsON, true, 4, 0);

   uint16_t countdown_time = 90; // 1 minuto y medio
   tick_t last_update = tickRead(); // Chequear que hace
   bool display_on = true;
    while(1) {
       // Calcula cuánto tiempo ha pasado desde la última actualización
        if (tickRead() - last_update >= 1000 && display_on) { //¿Pasó 1 segundo?
            last_update = tickRead(); //Reinicia el tiempo de referencia
            
            if (countdown_time > 0) {
                countdown_time--;
                tm1637_show_countdown(&display, countdown_time);
            } else {
                // Timer llegó a cero - hacer parpadear "00:00"
                static bool blink_state = true;
                
                if (blink_state) {
                    tm1637_show_countdown(&display, 0);
                } else {
                    tm1637_clear(&display);
                }
                blink_state = !blink_state;
            }
        }
        
        // Reiniciar timer con un botón (opcional)
        if (!gpioRead(TEC1)) {
            display_on = !display_on;
            if (display_on) {
               tm1637_set_brightness(&display, 7, true);  // Encender
            } else {
               tm1637_clear(&display); // Apagar
            }
            delay(300); // Debounce
        }
        
        delay(50);
    }

    return 0;
}