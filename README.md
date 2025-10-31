# Feedback drivers

## Buzzer 
* Tiene precargadas 5 tipos de melodías para representar todos los estados del juego. 
  ### Uso del driver
  * Se debe llamar a la función Buzzer_PlaySound pasándole como argumento una variable de tipo FeedbackState_t
    * void Buzzer_PlaySound(FeedbackState_t);
    * typedef enum {DISPARO_EXITOSO, WINNER, GAME_OVER, FEEDBACK_RESET, GAME} FeedbackState_t;
    #### SI VAN A COPIAR EL DRIVER @FEDE A LA CARPETA JUEGO, BORREN LA DECLARACIÓN DEL FEEDBACKSTATE_T DE ESTE HEADER