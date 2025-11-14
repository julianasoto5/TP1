- TM1637 -
Esta biblioteca permite controlar displays de 7 segmentos de 4 dígitos con controlador TM1637 desde la placa EDU-CIAA (LPC4337). Está implementada en C y utiliza las funciones de la sapi de la EDU-CIAA.

Conexiones (Display TM1637 → EDU-CIAA)
-CLK → GPIO5
-DIO → GPIO7
-VCC → +5V
-GND → GND

TM1637Display
Estructura principal que contiene la configuración del display. (Driver)
typedef struct {
    gpioMap_t pinClk;      // Pin del reloj
    gpioMap_t pinDIO;      // Pin de datos
    uint8_t brightness;    // Nivel de brillo (0-7)
    unsigned int bitDelay; // Retardo entre bits en µs
} TM1637Display;

Funciones Principales
  Inicialización y Configuración
    - tm1637_init: : Inicializa el display TM1637.
    - tm1637_set_brightness: Configura el brillo del display.
        - Nivel de brillo (0-7, donde 7 es el máximo)
        - on: true para encender, false para apagar
    - tm1637_clear: Limpia el display (apaga todos los segmentos).
    - tm1637_set_segments: Envía datos crudos de segmentos al display.
        - segments: Array de bytes con patrones de segmentos
        - length: Número de dígitos a actualizar (1-4)
        - pos: Posición inicial (0-3)

  Visualización de números
    - tm1637_show_number_dec (no existe mas): Muestra un número decimal.
        - num: Número a mostrar
        - leading_zero: true para mostrar ceros a la izquierda
    - tm1637_show_number_dec_ex: Muestra un número decimal con control de puntos.
        - dots: Máscara de bits para puntos (ver sección de puntos)
    - tm1637_show_number_hex_ex: Muestra un número hexadecimal.
    - tm1637_encode_digit: Convierte un dígito (0-15) en el patrón de segmentos correspondiente.
        - digit: Dígito a convertir (0-9: números, 10-15: A-F)
        Retorna: Patrón de bits para los segmentos

Funciones externas
- tickRead(): es una función de la sAPI (API estándar de la EDU-CIAA) que devuelve el tiempo transcurrido en milisegundos desde que se inició el programa. Retorna un valor de tipo tick_t (que generalmente es uint32_t) con el número de milisegundos desde el inicio.


Notas Importantes
Configuración de pines: Asegúrate de usar pines GPIO disponibles en la EDU-CIAA
Timing: El parámetro bitDelay puede necesitar ajustes según la longitud de los cables
Voltaje: El display requiere +5V para funcionar correctamente

Solución de Problemas
- Los números se ven distorsionados: Ajustar el bitDelay (probar con 50-200 µs)
- Los puntos no se encienden: Usar la máscara correcta en el parámetro dots. Verificar que se use tm1637_show_number_dec_ex en lugar de tm1637_show_number_dec