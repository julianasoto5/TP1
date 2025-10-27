/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef TAREA_MOVIMIENTO_H_
#define TAREA_MOVIMIENTO_H_

/*=====[Inclusions of public function dependencies]==========================*/

#include <stdint.h>
#include <stddef.h>

/*=====[Constantes de la Tarea de Movimiento]================================*/

// Constantes de Movimiento Fácil (Patrón de una dirección)
#define PASO_FACIL 1           // Cuántos grados mover en cada iteración
#define RETARDO_FACIL_MS 20    // Retardo entre pasos para velocidad constante (en ms)

// Constantes de Movimiento Difícil (Patrón Cuadrado/Rombo)
// --- AJUSTES PARA FLUIDEZ Y CONCURRENCIA ---
#define BASE_STEP_SIZE 1                     // **NUEVO: Usar 1 grado para máxima granularidad y suavidad.**
#define PASO_DIVISOR_FLUIDEZ 40              // **NUEVO: Divisor para calcular el retardo del paso (1200/40 = 30ms inicial)**
#define MIN_PASO_RETARDO_MS 2                // **NUEVO: Mínimo absoluto para la pausa interna.**
// ------------------------------------------

#define RETARDO_DIFICIL_MS_INICIAL 700      // Retardo inicial (lento) en ms (Tiempo total del movimiento)
#define ACELERACION_REDUCCION_MS 20          // Cantidad de ms a reducir en cada ciclo (aceleración)
#define RETARDO_MIN_DIFICIL_MS 400           // Retardo mínimo absoluto (velocidad máxima del juego)

typedef enum {
    M_ESQUINA_1_SUP_IZQ, // Mínimo X, Máximo Y
    M_ESQUINA_2_INF_IZQ, // Mínimo X, Mínimo Y
    M_ESQUINA_3_INF_DER, // Máximo X, Mínimo Y
    M_ESQUINA_4_SUP_DER  // Máximo X, Máximo Y
} MovimientoFase_t;
/*=====[Prototipos de funciones públicas]=====================================*/

void tarea_movimiento(void *pvParameters);


/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* TAREA_MOVIMIENTO_H_ */
