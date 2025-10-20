# TP1
***
* Autores:
  * Federica Montagna ()
  * Gabriela Herrera ()
  * Juliana Soto (03157/1)
  * Lola Dell'Oso ()
  
## Shooting game
* Esta carpeta contiene el firmware del proyecto Tiro al Blanco del Grupo 2 de la materia Taller de Proyecto 1. Se debe copiar en el path del firmware_v3 de epirson. 
  ### Planificador
    * game.c
  ### Drivers 
    * matrizLDR.c 
    * esp8266.c
    * servo.c
  
  ### Conexiones físicas a la EDU-CIAA
  * Las mismas se pueden modificar desde los archivos de cabecera correspondientes.
    * ESP8266: RC232_RX, RC232_TX, +5v, GND
    * Matriz LDR (4x4): ADC_CH1, ADC_CH2, ADC_CH3, DAC, TFIL_0, TFIL_1, TFIL_2, TCOL_0, AGND
    * Servo1: 
    * Servo2: