#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

/* Comentar para usar cambiar al modo mismatch */
#define ROTARY_ENCODER_THREADED

/* Pines del encoder*/
#define ENCODER_SW_PORT   GPIOC
#define ENCODER_SW_PIN    GPIO_PIN_13
#define ENCODER_DT_PORT   GPIOB
#define ENCODER_DT_PIN    GPIO_PIN_4
#define ENCODER_CLK_PORT  GPIOB
#define ENCODER_CLK_PIN   GPIO_PIN_5    /* Pin con interrupcion */

#define DEFAULT_LOWER_LIMIT   0
#define DEFAULT_UPPER_LIMIT   100
#define DEBOUNCE_DELAY_ROTARY 5    /* ms */
#define DEBOUNCE_DELAY_SWITCH 30   /* ms */

typedef struct {
    bool clockwiseStatus, counterclockwiseStatus;
    bool mismatchLastState, mismatchFlag;
    bool multiplierUsage;
    bool lastButtonState, buttonState;


    bool     lastCLK;
    uint32_t previousMillis;
    uint32_t lastDebounceTime;

    volatile int32_t position;
    int32_t lowerLimit, upperLimit, multiplier;
} RotaryEncoder_t;

void    RotaryEncoder_Init(RotaryEncoder_t *e);
void    RotaryEncoder_Callback(RotaryEncoder_t *e);      /* desde Interrupt de CLK (PC15) */
void    RotaryEncoder_SetPosition(RotaryEncoder_t *e, int32_t position);
int32_t RotaryEncoder_GetPosition(const RotaryEncoder_t *e);
bool    RotaryEncoder_ButtonPressed(RotaryEncoder_t *e);
void    RotaryEncoder_MismatchEngaged(RotaryEncoder_t *e);
void    RotaryEncoder_SetMultiplier(RotaryEncoder_t *e, int32_t multiplier, bool usage);
void    RotaryEncoder_UsageMultiplier(RotaryEncoder_t *e, bool usage);
void    RotaryEncoder_ChangeRange(RotaryEncoder_t *e, int32_t lower, int32_t upper);

extern RotaryEncoder_t g_encoder;

#endif /* ROTARY_ENCODER_H */
