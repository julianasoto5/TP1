#  Implementación del Mock de Hardware (ESP8266)

Esta rama (`feature/esp8266-mock`) contiene el código del firmware para el microcontrolador ESP8266, el cual actúa como un **servidor HTTP** para simular la funcionalidad de la EDU-CIAA. Su propósito es reemplazar al `mock-micro.js` y permitir al Servidor (`server/index.js`) obtener el estado del juego (`GET /status`) y enviar comandos (`POST /command`).

---

##  Configuración y Puesta en Marcha (Modo Simulación)

Para probar la integración del hardware con el Servidor y el Cliente:

### 1. Preparar el Firmware del ESP8266

1.  Abrir el archivo **`esp8266-mock.ino`** en el IDE de Arduino (o PlatformIO).
2.  Buscar la sección de configuración de red y **completar tus credenciales de Wi-Fi** (SSID y Password).
    ```cpp
    const char* ssid = "TU_WIFI_SSID";
    const char* password = "TU_WIFI_PASSWORD";
    ```
3.  Cargar el código en el ESP8266.

### 2. Obtener la IP del ESP8266

1.  Abrir el **Monitor Serial** (a 115200 baudios) mientras el ESP8266 se conecta.
2.  Una vez conectado, el microcontrolador imprimirá su **dirección IP local**. Esta es la dirección que de debe usar para el Servidor.

    > **Ejemplo:**
    >
    > `WiFi Conectado.`
    >
    > `ESP IP: 192.168.1.10`

### 3. Configurar el Servidor (Backend)

1.  Navegar a la carpeta `server/`.
2.  Abrir el archivo **`.env`**.
3.  Reemplazar el valor de `MICRO_URL` con la IP que se obtuvo en el paso anterior, manteniendo el puerto **`:5000`**.

    ```env
    # CAMBIAR ESTO:
    MICRO_URL=http://localhost:5000
    
    # POR ESTO (Ejemplo, usa tu IP real):
    MICRO_URL=[http://192.168.1.10:5000](http://192.168.1.10:5000) 
    ```

### 4. Ejecutar la Aplicación

Una vez configurado, el flujo de la aplicación pasa a ser: Cliente Web $\leftrightarrow$ Servidor $\leftrightarrow$ **ESP8266**.

1.  **NO levantar el Mock** (`node mock-micro.js`).
2.  Levantar el Servidor:
    ```bash
    cd server
    npm install
    npm run dev
    ```
3.  Levanta el Cliente Web:
    ```bash
    cd client
    npm install
    npm run dev
    ```

Al acceder a la aplicación web, el juego se comunicará directamente con el ESP8266, y al iniciar una partida, vse verá cómo el tiempo y el puntaje comienzan a simularse.
