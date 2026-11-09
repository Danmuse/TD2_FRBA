#include "rotaryEncoder.h"

RotaryEncoder_t g_encoder;

static inline bool rd(GPIO_TypeDef *port, uint16_t pin)
{
    return HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET;
}

/* Un paso de giro: up = horario. Aplica multiplicador y satura. */
static void step(RotaryEncoder_t *e, bool up)
{
    int32_t inc = e->multiplierUsage ? e->multiplier : 1;
    int32_t p   = e->position + (up ? inc : -inc);
    if (p > e->upperLimit) p = e->upperLimit;
    if (p < e->lowerLimit) p = e->lowerLimit;
    e->position = p;
}

void RotaryEncoder_Init(RotaryEncoder_t *e)
{
    e->clockwiseStatus     = true;
    e->mismatchLastState   = false;
    e->mismatchFlag        = false;
    e->multiplierUsage     = false;
    e->lastButtonState     = false;
    e->buttonState         = false;
    e->previousMillis      = 0;
    e->lastDebounceTime    = HAL_GetTick();
    e->position            = 0;
    e->multiplier          = 1;

    e->counterclockwiseStatus = rd(ENCODER_DT_PORT,  ENCODER_DT_PIN);
    e->lastCLK                = rd(ENCODER_CLK_PORT, ENCODER_CLK_PIN);
    RotaryEncoder_ChangeRange(e, DEFAULT_LOWER_LIMIT, DEFAULT_UPPER_LIMIT);
}

#ifdef ROTARY_ENCODER_THREADED
void RotaryEncoder_Callback(RotaryEncoder_t *e)
{
    bool currentCLK = rd(ENCODER_CLK_PORT, ENCODER_CLK_PIN);

    if (currentCLK && !e->lastCLK) {                       /* flanco de subida */
        bool data = rd(ENCODER_DT_PORT, ENCODER_DT_PIN);
        step(e, data != currentCLK);                       /* DT=0 -> horario */
    }
    e->lastCLK = currentCLK;
}
#else
void RotaryEncoder_Callback(RotaryEncoder_t *e)
{
    e->clockwiseStatus = rd(ENCODER_CLK_PORT, ENCODER_CLK_PIN);
    bool mm = e->mismatchFlag ? (e->clockwiseStatus != e->mismatchLastState) : false;

    if ((HAL_GetTick() - e->previousMillis) > DEBOUNCE_DELAY_ROTARY) {
        e->previousMillis = HAL_GetTick();
        e->mismatchFlag   = false;

        bool changed = mm ? (e->clockwiseStatus == e->counterclockwiseStatus)
                          : (e->clockwiseStatus != e->counterclockwiseStatus);
        if (changed) {
            step(e, rd(ENCODER_DT_PORT, ENCODER_DT_PIN) != e->clockwiseStatus);
        }
        e->counterclockwiseStatus = e->clockwiseStatus;
    }
}
#endif

void RotaryEncoder_MismatchEngaged(RotaryEncoder_t *e)
{
    e->mismatchLastState = rd(ENCODER_CLK_PORT, ENCODER_CLK_PIN);
    e->mismatchFlag = true;
}

bool RotaryEncoder_ButtonPressed(RotaryEncoder_t *e)
{
    bool cur = !rd(ENCODER_SW_PORT, ENCODER_SW_PIN);       /* pull-up: apretado = 0 */

    if (cur != e->lastButtonState)
    {
        e->lastDebounceTime = HAL_GetTick();
        e->lastButtonState  = cur;
    }
    if ((HAL_GetTick() - e->lastDebounceTime) > DEBOUNCE_DELAY_SWITCH &&
        cur != e->buttonState)
    {
        e->buttonState = cur;
        return cur;                                        /* true solo al apretar */
    }
    return false;
}

void RotaryEncoder_SetPosition(RotaryEncoder_t *e, int32_t position)
{
    if (position < e->lowerLimit)      e->position = e->lowerLimit;
    else if (position > e->upperLimit) e->position = e->upperLimit;
    else                               e->position = position;
}

int32_t RotaryEncoder_GetPosition(const RotaryEncoder_t *e) { return e->position; }

void RotaryEncoder_UsageMultiplier(RotaryEncoder_t *e, bool usage)
{
    e->multiplierUsage = usage;
}

void RotaryEncoder_SetMultiplier(RotaryEncoder_t *e, int32_t m, bool usage)
{
    if (m < e->lowerLimit)      e->multiplier = e->lowerLimit + 1;
    else if (m > e->upperLimit) e->multiplier = e->upperLimit - 1;
    else                        e->multiplier = m;
    RotaryEncoder_UsageMultiplier(e, usage);
}

void RotaryEncoder_ChangeRange(RotaryEncoder_t *e, int32_t lower, int32_t upper)
{
    e->lowerLimit = lower;
    e->upperLimit = upper;
    if (e->position < lower)      e->position = lower;
    else if (e->position > upper) e->position = upper;
}
