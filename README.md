# Juego Interactivo "Tiro al Blanco": Diana con Detección Fotoeléctrica

Proyecto desarrollado para la asignatura **Taller de Proyecto 1** de la **Facultad de Ingeniería - Universidad Nacional de La Plata (UNLP)**.

Consiste en un prototipo interactivo de tiro al blanco compuesto por una diana móvil con sensores fotoeléctricos, una pistola láser, un sistema de retroalimentación audiovisual y una aplicación web para la gestión de partidas y el registro de mejores puntajes.

---

## Características Principales

* **Detección Fotoeléctrica:** Matriz de 12 fotorresistencias (LDR GL5528) distribuidas en 3 anillos concéntricos que asignan distintos puntajes según el área alcanzada.
* **Movimiento Bidimensional (2D):** Control de movimiento en 2 ejes mediante servomotores (SG90 para horizontal y MG90S para vertical).
* **Sistema Embebido Multitarea:** Programado en C sobre la plataforma **EDU-CIAA-NXP** (LPC4337) haciendo uso del sistema operativo de tiempo real **FreeRTOS**.
* **Conectividad Wi-Fi:** Comunicación serie UART entre la EDU-CIAA y un módulo **NodeMCU ESP8266**, el cual expone un punto de acceso (Access Point `ESP8266-Juego`) con endpoints HTTP.
* **Aplicación Web:** Interfaz gráfica desarrollada en React con servidor intermedio en Node.js/Express para la selección de nivel de dificultad, visualización de partida y ranking local persistente.
* **Retroalimentación Audiovisual:** Display TM1637 de 4 dígitos para el tiempo restante, zumbador piezoeléctrico pasivo para efectos sonoros y melodías, y tira de LEDs de alto brillo para estado visual.

---

## Arquitectura del Sistema

El sistema se organiza en 5 bloques físicos y lógicos principales:

1. **Unidad de Control Central:** Placa EDU-CIAA-NXP responsable de la lógica general y el firmware, junto al módulo ESP8266 para la interfaz Wi-Fi.
2. **Unidad de Diana Móvil:** Matriz LDR con diodos 1N4148 y resistores pull-down junto con la estructura física impulsada por los servomotores SG90 y MG90S.
3. **Pistola:** Puntero láser rojo de baja potencia accionado mediante pulsador.
4. **Unidad de Retroalimentación:** Display de 7 segmentos TM1637, zumbador piezoeléctrico y tira LED accionada mediante transistores NPN 2N2222.
5. **Aplicación Web:** Interfaz para el usuario que se comunica bidireccionalmente por solicitudes HTTP/JSON.

---

## Estructura del Firmware y Software

### Firmware (EDU-CIAA-NXP / FreeRTOS)
El firmware adopta una arquitectura multitarea orientada a eventos:
* **Capa de Drivers (HAL):** Controladores de periféricos (`matrizLDR.c`, `servo.c`, `displayTM1637.c`, `buzzer.c`, `leds.c`, `esp8266.c`).
* **Capa RTOS:** Gestión de prioridades, temporizadores e interrupciones (UART y Timer 10ms), sincronización mediante colas y notificaciones.
* **Capa de Aplicación (Tareas):**
  * `Tarea_Juego`: Máquina de estados principal (`IDLE`, `READY`, `PLAYING`, `GAME OVER`, `GAME RESET`).
  * `Tarea_Sensores`: Escaneo secuencial fila-columna sobre los canales del ADC.
  * `Tarea_Movimiento`: Generación de patrones de movimiento según el nivel seleccionado.
  * `Tarea_Comunicacion`: Procesamiento de comandos y envío de datos vía UART/Wi-Fi.
  * `Tarea_Feedback`: Ejecución de alertas lumínicas y sonoras.

### Aplicación Web
* **Frontend:** React.
* **Backend:** Node.js con Express (Servidor API REST HTTP).

---

## Hardware y Diseño Electrónico

* **Controlador Principal:** EDU-CIAA-NXP (ARM Cortex-M4 LPC4337).
* **PCB Shield (Poncho):** Placa monocapa diseñada en **KiCad** compatible con la EDU-CIAA.
* **Alimentación General:** Fuente conmutada (switching) de 5V – 3A.

---

## Modos de Juego y Niveles

* **Nivel FÁCIL:** Movimiento unidireccional (lateral) de la diana a velocidad constante.
* **Nivel DIFÍCIL:** Movimiento aleatorio bidimensional (X e Y) con velocidad variable.

---

## Instrucciones de Instalación y Uso

### Requisitos Previos
* Node.js instalado en el equipo.
* Navegador Web moderno (Google Chrome o Mozilla Firefox).

### Pasos para Ejecutar

1. **Clonar el repositorio:**
   ```bash
   git clone [https://github.com/julianasoto5/TP1.git](https://github.com/julianasoto5/TP1.git)
   cd TP1
   ```

2. **Iniciar la aplicación:**
   ```bash
   npm run dev
   ```

3. **Conectar la PC a la red Wi-Fi:**
   Buscar y conectarse a la red Wi-Fi emitida por el prototipo: `ESP8266-Juego`

4. **Abrir la interfaz:**
   Acceder desde el navegador a la dirección:
   ```text
   http://localhost:5173/
   ```

---

## Integrantes del Proyecto

* **Dell'Oso, Lola** — *Diseño mecánico, PCB y montaje*.
* **Herrera, Gabriela** — *Esquemático, PCB y ensamblado electrónico*.
* **Montagna, Federica** — *Arquitectura de software, firmware RTOS y aplicación web*.
* **Soto, Juliana** — *Diseño de hardware, firmware, ensamble y pruebas de integración*.