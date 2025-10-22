# TP1
***
* Autores:
  * Federica Montagna ()
  * Gabriela Herrera ()
  * Juliana Soto (03157/1)
  * Lola Dell'Oso ()
  
## Juego
* Esta carpeta contiene el firmware del proyecto Tiro al Blanco del Grupo 2 de la materia Taller de Proyecto 1. Se debe copiar en el path del firmware_v3 de epirson. 
  
  ### Planificador
  * scheduler.c
  
  ### Tareas
  * _userTasks.h_: contiene prototipos de funciones
  * _tarea_juego.c_ : contiene lógica de juego
  * _tarea_sensores.c_: constituye la tarea de sensado de los LDR
  * _tarea_movimiento.c_: realiza movimientos (que pueden ser aleatorios) de acuerdo al nivel de juego
  * _tarea_feedback.c_: encargado de generar el feedback "físico" al usuario
  * _tarea_comunicacion.c_: encargado de la comunicación entre EDU-CIAA y ESP8266

  ### Drivers 
  * _matrizLDR.c_: funciones básicas para sensado de LDR 
  * _esp8266.c_: funciones básicas para comunicación vía WiFi 
  * _servo.c_: funciones básicas para generar movimientos del servo SG90
  * _tm1637.c_: funciones básicas para mostrar en el display el tiempo

  ### Archivos necesarios
  * _config.mk_: está modificado para que compile los archivos del FreeRTOS, y además para que incluya en la compilación a los archivos ubicados en src/drivers y src/tareas. Además, se deben habilitar las interrupciones para el uso de los servos.
    
    ```bash
    USE_FREERTOS=y
    FREERTOS_HEAP_TYPE=4

    DEFINES+=SAPI_USE_INTERRUPTS
    
    SRC += $(wildcard $(PROGRAM_PATH_AND_NAME)/src/drivers/*.c)
    SRC += $(wildcard $(PROGRAM_PATH_AND_NAME)/src/tareas/*.c)
    ```
  * _FreeRTOSConfig.h_: Este archivo contiene todas las constantes necesarias para que el kernel utilice un esquema de RTOS. Está configurado especialmente para manejar un algoritmo de planificación de **Round Robin**.
  
  ### Conexiones físicas a la EDU-CIAA
  * Las mismas se pueden modificar desde los archivos de cabecera correspondientes.
    * ESP8266: RC232_RX, RC232_TX, +5v, GND
    * Matriz LDR (4x4): ADC_CH1, ADC_CH2, ADC_CH3, TCOL_1, TFIL_0, TFIL_1, TFIL_2, TCOL_0, AGND
    * Servo1: GPIO8, GND
    * Servo2: GPIO2, GND

  ### Comentarios
  * Al ejecutar el archivo scheduler.c tal como está ahora, se generan 3 tareas: tarea_sensores, tarea_juego y tarea_movimiento. La tarea_juego está configurada para empezar en el estado STATE_PLAYING, mientras que la tarea_movimiento todavía está en desarrollo (*ver comentarios en el archivo tarea_movimiento.c*). La tarea_sensores ya está completa y funciona correctamente. Parece que la tarea_juego se queda esperando en los envíos xQueueSend(xColaFeedback, &msg, 0); (linea 112) y xQueueSend(xColaMovimiento, &msg, 0); (linea 115), aunque creo que no debería estar pasando eso. Para testear la integración entre tarea_juego y tarea_sensores, con comentar esas dos líneas es suficiente.