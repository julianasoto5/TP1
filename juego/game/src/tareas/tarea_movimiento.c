#include "servo.h"
#include "userTasks.h" 
#include "tarea_juego.h" 
#include "task.h" 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // Necesario para rand() y abs()
#include "tarea_movimiento.h" // Constantes de movimiento

extern QueueHandle_t xColaMovimiento; // Cola para comunicación con Tarea Juego

// Variables ESTÁTICAS para mantener el estado entre llamadas
static int8_t s_dir_x = 1;      // Dirección: 1 (aumenta), -1 (disminuye)
static int8_t s_dir_y = 1;
static int16_t s_pos_x = POS_INICIAL_GRADOS;
static int16_t s_pos_y = POS_INICIAL_GRADOS;
static uint16_t s_retardo_actual_ms = RETARDO_DIFICIL_MS_INICIAL; 

static MovimientoFase_t s_fase_actual = M_ESQUINA_1_SUP_IZQ; 

/* Funciones de control de motores  */

static void PonerDianaPosicionInicial(void) {
    Servo_PonerInicial();
    
    // Reinicializar el estado de la lógica para el próximo juego
    s_pos_x = POS_INICIAL_GRADOS;
    s_pos_y = POS_INICIAL_GRADOS;
    s_dir_x = 1;
    s_dir_y = 1;
    s_retardo_actual_ms = RETARDO_DIFICIL_MS_INICIAL; 
    s_fase_actual = M_ESQUINA_1_SUP_IZQ; // Reiniciar fase
}


static void MoverDianaFacil(void) {
    // 1. Lógica: Mover en un solo eje (X)
    s_pos_x += PASO_FACIL * s_dir_x;

    // 2. Comprobar límites y cambiar la dirección 
    if (s_pos_x >= ANGULO_MAX_GRADOSX) {
        s_pos_x = ANGULO_MAX_GRADOSX;
        s_dir_x = -1; 
    } else if (s_pos_x <= ANGULO_MIN_GRADOSX) {
        s_pos_x = ANGULO_MIN_GRADOSX;
        s_dir_x = 1;  
    }

    // 3. Actuar sobre el driver 
    Servo_SetPosicionX((uint8_t)s_pos_x);
    Servo_SetPosicionY(POS_INICIAL_GRADOS); // Y se mantiene fijo
    
    // 4. Esperar para velocidad constante
    vTaskDelay(pdMS_TO_TICKS(RETARDO_FACIL_MS));
}


// Lógica para mover la diana en un patrón fijo de CUADRADO, progresivamente más rápido.
static void MoverDianaDificil(void) {
    
    int16_t destino_x;
    int16_t destino_y;
    
    // 1. Determinar el nuevo destino basado en la fase actual
    switch (s_fase_actual) {
        case M_ESQUINA_1_SUP_IZQ: 
            destino_x = ANGULO_MIN_GRADOSX; 
            destino_y = ANGULO_MAX_GRADOSY;
            break;
            
        case M_ESQUINA_2_INF_IZQ: 
            destino_x = ANGULO_MIN_GRADOSX;
            destino_y = ANGULO_MIN_GRADOSY;
            break;
            
        case M_ESQUINA_3_INF_DER: 
            destino_x = ANGULO_MAX_GRADOSX;
            destino_y = ANGULO_MIN_GRADOSY; 
            break;
            
        case M_ESQUINA_4_SUP_DER: 
            destino_x = ANGULO_MAX_GRADOSX;
            destino_y = ANGULO_MAX_GRADOSY;
            break;
    }
    
    // 2. Configurar parámetros de movimiento
    int16_t actual_x = s_pos_x;
    int16_t actual_y = s_pos_y;
    
    // Dirección del paso (+1 o -1)
    int16_t dir_x = (destino_x > actual_x) ? 1 : -1;
    int16_t dir_y = (destino_y > actual_y) ? 1 : -1;
    
    // Retardo para la interpolación (fluidez). Se reduce a medida que el juego acelera.
    uint16_t retardo_paso_ms = s_retardo_actual_ms / PASO_DIVISOR_FLUIDEZ; 
    
    // Aseguramos el mínimo absoluto para el retardo del paso (2ms)
    if (retardo_paso_ms < MIN_PASO_RETARDO_MS) retardo_paso_ms = MIN_PASO_RETARDO_MS; 
    
    // 3. Bucle de Movimiento Concurrente (Diagonal fluido con pasos dinámicos)
    // El bucle sigue mientras no se haya llegado al destino en ambos ejes
    while (actual_x != destino_x || actual_y != destino_y) {
        
        // --- Cálculo Dinámico del Paso X ---
        if (actual_x != destino_x) {
            // Distancia restante a mover en X
            int16_t dist_restante_x = abs(destino_x - actual_x); 
            // El paso real es el mínimo entre el paso base (1) y la distancia restante
            int16_t paso_abs_x = (dist_restante_x > BASE_STEP_SIZE) ? BASE_STEP_SIZE : dist_restante_x;
            
            // Mover un paso
            actual_x += (dir_x * paso_abs_x); 
            
            // Aseguramos que no sobrepasa el destino
            if ((dir_x > 0 && actual_x > destino_x) || (dir_x < 0 && actual_x < destino_x)) {
                 actual_x = destino_x;
            }
            Servo_SetPosicionX((uint8_t)actual_x); 
        }

         vTaskDelay(1); // Espera 1 tick
        // --- Cálculo Dinámico del Paso Y ---
        if (actual_y != destino_y) {
            // Distancia restante a mover en Y
            int16_t dist_restante_y = abs(destino_y - actual_y);
            // El paso real es el mínimo entre el paso base (1) y la distancia restante
            int16_t paso_abs_y = (dist_restante_y > BASE_STEP_SIZE) ? BASE_STEP_SIZE : dist_restante_y;
            
            // Mover un paso
            actual_y += (dir_y * paso_abs_y);
            
            // Aseguramos que no sobrepasa el destino
            if ((dir_y > 0 && actual_y > destino_y) || (dir_y < 0 && actual_y < destino_y)) {
                 actual_y = destino_y;
            }
            Servo_SetPosicionY((uint8_t)actual_y);
        }
        
        // Cede la CPU
        vTaskDelay(pdMS_TO_TICKS(retardo_paso_ms));
    }
    
    // 4. Actualizar la posición global
    s_pos_x = destino_x;
    s_pos_y = destino_y;
    
    // 5. Transición al siguiente estado
    s_fase_actual = (s_fase_actual + 1) % 4; // Hay 4 fases (M_ESQUINA_1 a M_ESQUINA_4)
    
    // 6. Aumentar la velocidad progresivamente (Aceleración)
    if (s_retardo_actual_ms > RETARDO_MIN_DIFICIL_MS) {
        s_retardo_actual_ms -= ACELERACION_REDUCCION_MS; 
    } else {
        s_retardo_actual_ms = RETARDO_MIN_DIFICIL_MS;
    }
}

/* Tarea Principal */
void tarea_movimiento(void *pvParameters)
{
    GameEvent_t msg;
    uint8_t nivel = -1;
    bool moviendo = false;
    Servo_Init();
    srand((unsigned int)xTaskGetTickCount());
    
    // Bucle principal: Espera por la señal de inicio de juego
    while (1) {

        if (!moviendo) {
            printf("[MOVIMIENTO] En espera de evento START (NIVEL).\r\n");
            // Esperar un mensaje de inicio de la tarea Juego
            if (xQueueReceive(xColaMovimiento, &msg, portMAX_DELAY) == pdPASS) {
                if (msg.tipo == START_GAME) {
                    nivel = msg.valor;
                    moviendo = true;
                    s_retardo_actual_ms = RETARDO_DIFICIL_MS_INICIAL; 
                    printf("[MOVIMIENTO] Partida iniciada. Nivel: %u. Iniciando movimiento.\r\n", nivel);
                }
            }
        } else {
            // Se comprueba si hay un mensaje STOP (sin bloquear)
            if (xQueueReceive(xColaMovimiento, &msg, 0) == pdPASS) { 
                if (msg.tipo == FIN_PARTIDA) { 
                    printf("[MOVIMIENTO] Evento STOP recibido. Deteniendo movimiento.\r\n");
                    PonerDianaPosicionInicial(); 
                    moviendo = false;
                    nivel = -1;
                    continue; 
                }
            }
            if (nivel == 0) { 
                MoverDianaFacil();
            } else if (nivel >= 1) { 
                MoverDianaDificil();
            }
        } 
    }
}
