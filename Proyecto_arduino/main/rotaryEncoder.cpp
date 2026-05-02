/*/*!
 * @file rotaryEncoder.cpp
 * @par Author & Doxygen Editor
 *  Daniel Di Módica ~ <a href = "mailto: danifabriziodmodica@gmail.com">danifabriziodmodica@@gmail.com</a>
 * @date 16/03/2024 16:05:47
 * @version 1.0
 */

#include "rotaryEncoder.h"

RotaryEncoder::RotaryEncoder(byte clockPin, byte dataPin) :
m_clockwiseRotationStatus{true},
m_counterclockwiseRotationStatus{true},
m_mismatchLastState{false},
m_mismatchFlag{false},
m_multiplierUsage{false},
m_buttonUsage{false},
m_clockPin{clockPin},
m_dataPin{dataPin},
m_position{0},
m_multiplier{1} {
  pinMode(this->m_clockPin, INPUT);
  pinMode(this->m_dataPin, INPUT);
  this->m_counterclockwiseRotationStatus = digitalRead(this->m_dataPin);
  this->changeRange();
}

RotaryEncoder::RotaryEncoder(byte buttonPin, byte clockPin, byte dataPin) :
m_clockwiseRotationStatus{true},
m_counterclockwiseRotationStatus{true},
m_mismatchLastState{false},
m_mismatchFlag{false},
m_multiplierUsage{false},
m_lastButtonState{false},
m_buttonState{false},
m_buttonUsage{true},
m_buttonPin{buttonPin},
m_clockPin{clockPin},
m_dataPin{dataPin},
m_position{0},
m_multiplier{1} {
  pinMode(this->m_buttonPin, INPUT_PULLUP);
  pinMode(this->m_clockPin, INPUT);
  pinMode(this->m_dataPin, INPUT);
  this->m_counterclockwiseRotationStatus = digitalRead(this->m_dataPin);
  this->changeRange();
}

#ifndef ROTARY_ENCODER_THREADED
void RotaryEncoder::rotaryEncoderCallback(void) {
  static uint32_t previousMillis = 0;
  this->m_clockwiseRotationStatus = digitalRead(this->m_clockPin);
  bool mismatchFlag = this->m_mismatchFlag ? this->m_clockwiseRotationStatus != this->m_mismatchLastState : false; 
  if ((millis() - previousMillis) > DEBOUNCE_DELAY_ROTARY) {
    previousMillis = millis();
    this->m_mismatchFlag = false;
    if (!mismatchFlag) {
      if (this->m_clockwiseRotationStatus != this->m_counterclockwiseRotationStatus /* && this->m_clockwiseRotationStatus == true */) {
        if (digitalRead(this->m_dataPin) != this->m_clockwiseRotationStatus) {
          if (this->m_multiplierUsage) {
            if (this->m_position + this->m_multiplier < this->m_upperLimit) this->m_position += this->m_multiplier;
            else this->m_position = this->m_upperLimit;
          } else { if (this->m_position < this->m_upperLimit) this->m_position++; }
        } else {
          if (this->m_multiplierUsage) {
            if (this->m_position - this->m_multiplier > this->m_lowerLimit) this->m_position -= this->m_multiplier;
            else this->m_position = this->m_lowerLimit;
          } else { if (this->m_position > this->m_lowerLimit) this->m_position--; }
        }
      }
    } else {
      if (this->m_clockwiseRotationStatus == this->m_counterclockwiseRotationStatus /* && this->m_clockwiseRotationStatus == true */) {
        if (digitalRead(this->m_dataPin) != this->m_clockwiseRotationStatus) {
          if (this->m_multiplierUsage) {
            if (this->m_position + this->m_multiplier < this->m_upperLimit) this->m_position += this->m_multiplier;
            else this->m_position = this->m_upperLimit;
          } else { if (this->m_position < this->m_upperLimit) this->m_position++; }
        } else {
          if (this->m_multiplierUsage) {
            if (this->m_position - this->m_multiplier > this->m_lowerLimit) this->m_position -= this->m_multiplier;
            else this->m_position = this->m_lowerLimit;
          } else { if (this->m_position > this->m_lowerLimit) this->m_position--; }
        }
      }
    }
    this->m_counterclockwiseRotationStatus = this->m_clockwiseRotationStatus;
  }
}
#else
void RotaryEncoder::rotaryEncoderCallback(void) {
  static bool lastCLK = LOW;

  bool currentCLK = digitalRead(m_clockPin);

  // Detectar flanco de subida
  if (currentCLK && !lastCLK) {
    bool data = digitalRead(m_dataPin);

    if (data != currentCLK) {
      // Giro horario
      if (m_position < m_upperLimit) m_position++;
    } else {
      // Giro antihorario
      if (m_position > m_lowerLimit) m_position--;
    }
  }

  lastCLK = currentCLK;
}
/*
  static uint32_t previousMillis = 0;
  this->m_clockwiseRotationStatus = digitalRead(this->m_clockPin);
  bool mismatchFlag = this->m_mismatchFlag ? this->m_clockwiseRotationStatus != this->m_mismatchLastState : false; 
  if ((millis() - previousMillis) > DEBOUNCE_DELAY_ROTARY) {
    previousMillis = millis();
    this->m_mismatchFlag = false;
    if (!mismatchFlag) {
      if (this->m_clockwiseRotationStatus != this->m_counterclockwiseRotationStatus && this->m_clockwiseRotationStatus) {
        if (digitalRead(this->m_dataPin) != this->m_clockwiseRotationStatus) {
          if (this->m_multiplierUsage) {
            if (this->m_position + this->m_multiplier < this->m_upperLimit) this->m_position += this->m_multiplier;
            else this->m_position = this->m_upperLimit;
          } else { if (this->m_position < this->m_upperLimit) this->m_position++; }
        } else {
          if (this->m_multiplierUsage) {
            if (this->m_position - this->m_multiplier > this->m_lowerLimit) this->m_position -= this->m_multiplier;
            else this->m_position = this->m_lowerLimit;
          } else { if (this->m_position > this->m_lowerLimit) this->m_position--; }
        }
      }
    } else {
      if (this->m_clockwiseRotationStatus == this->m_counterclockwiseRotationStatus && this->m_clockwiseRotationStatus) {
        if (digitalRead(this->m_dataPin) != this->m_clockwiseRotationStatus) {
          if (this->m_multiplierUsage) {
            if (this->m_position + this->m_multiplier < this->m_upperLimit) this->m_position += this->m_multiplier;
            else this->m_position = this->m_upperLimit;
          } else { if (this->m_position < this->m_upperLimit) this->m_position++; }
        } else {
          if (this->m_multiplierUsage) {
            if (this->m_position - this->m_multiplier > this->m_lowerLimit) this->m_position -= this->m_multiplier;
            else this->m_position = this->m_lowerLimit;
          } else { if (this->m_position > this->m_lowerLimit) this->m_position--; }
        }
      }
    }
    this->m_counterclockwiseRotationStatus = this->m_clockwiseRotationStatus;
  }
}
*/
#endif

void RotaryEncoder::setPosition(int32_t position) {
  if (position < this->m_lowerLimit) this->m_position = this->m_lowerLimit;
  else if (position > this->m_upperLimit) this->m_position = this->m_upperLimit;
  else this->m_position = position;
}

int32_t RotaryEncoder::getPosition(void) const {
  return this->m_position;
}

bool RotaryEncoder::buttonPressed(void) {
  if (!this->m_buttonUsage) return false;
  static uint32_t lastDebounceTime = 0;
#if defined(ESP8266) || (__STM32F1__)
  // return !digitalRead(this->m_buttonPin);
  bool currentButtonState = !digitalRead(this->m_buttonPin);
  if (currentButtonState != this->m_lastButtonState) lastDebounceTime = millis();
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_SWITCH) {
    if (currentButtonState != this->m_buttonState) {
      this->m_buttonState = currentButtonState;
      return this->m_buttonState;
    }
  }
  this->m_lastButtonState = currentButtonState;
  return false;
#else
  // return digitalRead(this->m_buttonPin);
  bool currentButtonState = digitalRead(this->m_buttonPin);
  if (currentButtonState != this->m_lastButtonState) lastDebounceTime = millis();
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_SWITCH) {
    if (currentButtonState != this->m_buttonState) {
      this->m_buttonState = currentButtonState;
      return this->m_buttonState;
    }
  }
  this->m_lastButtonState = currentButtonState;
  return false;
#endif
}

void RotaryEncoder::mismatchEngaged(void) {
  this->m_mismatchLastState = digitalRead(this->m_clockPin);
  this->m_mismatchFlag = true;
}

void RotaryEncoder::setMultiplier(int32_t multiplier, bool multiplierUsage) {
  if (multiplier < this->m_lowerLimit) this->m_multiplier = this->m_lowerLimit + 1;
  else if (multiplier > this->m_upperLimit) this->m_multiplier = this->m_upperLimit - 1;
  else this->m_multiplier = multiplier;
  this->usageMultiplier(multiplierUsage);
}

void RotaryEncoder::usageMultiplier(bool multiplierUsage) {
  this->m_multiplierUsage = multiplierUsage ? true : false;
}

void RotaryEncoder::changeRange(int32_t lowerLimit, int32_t upperLimit) {
  this->m_lowerLimit = lowerLimit;
  this->m_upperLimit = upperLimit;
  if (this->m_position < lowerLimit) this->m_position = lowerLimit;
  else if (this->m_position > upperLimit) this->m_position = upperLimit;
}

RotaryEncoder g_encoder(GPIO_KY040_SW_PIN, GPIO_KY040_CLK_PIN, GPIO_KY040_DT_PIN);
