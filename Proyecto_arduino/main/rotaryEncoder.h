/*!
 * @file rotaryEncoder.h
 * @par Author & Doxygen Editor
 *  Daniel Di Módica ~ <a href = "mailto: danifabriziodmodica@gmail.com">danifabriziodmodica@@gmail.com</a>
 * @date 16/03/2024 16:05:47
 * @version 1.0
 * @brief Class for controlling KY040 Rotary Encoder
 * @details @par Description
 *  The <b>RotaryEncoder</b> class provides a set of functions for interfacing with and controlling the KY040 Incremental rotary encoder. @n
 *  This class enables users to easily monitor and handle rotational movements of the encoder, allowing for precise input control in various applications. @n
 *  It abstracts the low-level interactions with the rotary encoder, providing a high-level interface for detecting changes in position, handling rotation events, @n
 *  and implementing custom functionality based on the encoder's input.
 */

#ifndef _ROTARY_ENCODER_H_
#define _ROTARY_ENCODER_H_

#include "utils.h"

// Comment or uncomment the following macro depending on the type of encoder used
#define ROTARY_ENCODER_THREADED

#define DEFAULT_LOWER_LIMIT 0
#define DEFAULT_UPPER_LIMIT 100

#define DEBOUNCE_DELAY_ROTARY 5
#define DEBOUNCE_DELAY_SWITCH 30

class RotaryEncoder {
  public:
    RotaryEncoder() = delete;
    RotaryEncoder(byte clockPin, byte dataPin);
    RotaryEncoder(byte buttonPin, byte clockPin, byte dataPin);

    void rotaryEncoderCallback(void);
    void setPosition(int32_t position);
    int32_t getPosition(void) const;
    bool buttonPressed(void);
    void mismatchEngaged(void);
    void setMultiplier(int32_t multiplier, bool multiplierUsage = true);
    void usageMultiplier(bool multiplierUsage);
    void changeRange(int32_t lowerLimit = DEFAULT_LOWER_LIMIT, int32_t upperLimit = DEFAULT_UPPER_LIMIT);
    
    virtual ~RotaryEncoder() = default;
  private:
    bool m_clockwiseRotationStatus, m_counterclockwiseRotationStatus, m_mismatchLastState, m_mismatchFlag, m_multiplierUsage, m_lastButtonState, m_buttonState, m_buttonUsage;
    byte m_buttonPin, m_clockPin, m_dataPin;
    int32_t m_lowerLimit, m_upperLimit, m_position, m_multiplier;
};

extern RotaryEncoder g_encoder;

#endif // _ROTARY_ENCODER_H_
