/*/*!
 * @file AD9833_DDS.cpp
 * @par Author & Doxygen Editor
 *  Daniel Di Módica ~ <a href = "mailto: danifabriziodmodica@gmail.com">danifabriziodmodica@@gmail.com</a>
 * @date 29/09/2024 13:40:36
 * @version 1.0
 */

#include "AD9833_DDS.h"

SPISettings AD9833_SPISettings(AD9833_DDS_SPI_SPEED, MSBFIRST, SPI_MODE2, DATA_SIZE_8BIT);
AD9833_DDS g_signalGenerator(GPIO_AD9833_FSYNC);

AD9833_DDS::AD9833_DDS(uint8_t slaveSelect) :
m_selectPin{slaveSelect} { }

void AD9833_DDS::reset(void) {
  // this->writeRegister(AD9833_RESET);
  this->setWave(SIG_NONE, FREG0);
  this->setWave(SIG_NONE, FREG1);
  // Reset all library variables to be in "sync" with hardware.
  this->m_freq[0] = this->m_freq[1] = 0;
  this->m_phase[0] = this->m_phase[1] = 0;
  this->m_wave[0] = this->m_wave[1] = SIG_NONE;
}

void AD9833_DDS::writeRegister(uint16_t data) {
  digitalWrite(this->m_selectPin, LOW);
  delayMicroseconds(1);
  // SPI.transfer16(data); // NOTE: Uses 32 bytes more than the instructions below
  SPI.transfer(data >> 8);
  SPI.transfer(data & 0xFF);
  digitalWrite(this->m_selectPin, HIGH);
}

void AD9833_DDS::begin(void) {
  pinMode(this->m_selectPin, OUTPUT);
  digitalWrite(this->m_selectPin, HIGH);
  spiTransaction(AD9833_SPISettings, []() { });
  this->reset();
}

void AD9833_DDS::setFrequency(uint32_t frequency, freqReg_t freqReg) {
  uint32_t freqWord = (frequency > AD9833_DDS_MAX_FREQ ? AD9833_DDS_MAX_FREQ : frequency) * CRYSTAL_FREQ_FACTOR;
  uint16_t MSB = ((uint16_t)((freqWord & 0xFFFC000) >> 14)) | freqReg;
  uint16_t LSB = ((uint16_t)(freqWord & 0x3FFF)) | freqReg;
  this->writeRegister(AD9833_B28);
  this->writeRegister(LSB);
  this->writeRegister(MSB);
  this->m_freq[freqReg == FREG0 ? 0 : 1] = freqWord / CRYSTAL_FREQ_FACTOR;
}

void AD9833_DDS::setWave(signal_type_t signalType, freqReg_t freqReg) {
  uint16_t controlRegister;
  if (signalType == SIG_NONE) {
    this->writeRegister(AD9833_RESET);
    controlRegister = (uint16_t)(WAVE_NONE);
  } else if (signalType == SIG_SINE) controlRegister = (uint16_t)(WAVE_SINE);
  else if (signalType == SIG_FST_SQUARE) controlRegister = (uint16_t)(WAVE_SQUARE1);
  else if (signalType == SIG_SND_SQUARE) controlRegister = (uint16_t)(WAVE_SQUARE2);
  else if (signalType == SIG_TRIANGLE) controlRegister = (uint16_t)(WAVE_TRIANGLE);
  this->writeRegister(freqReg == FREG0 ? 0xC000 : 0xE000);
  this->writeRegister(controlRegister);
  this->m_wave[freqReg == FREG0 ? 0 : 1] = signalType;
}

/*
void AD9833_DDS::setPhase(uint16_t phase, freqReg_t freqReg) {
  uint16_t phaseWord; 
  while (phaseWord >= AD9833_DSS_MAX_PHASE) phaseWord -= AD9833_DSS_MAX_PHASE;
  while (phaseWord < 0) phaseWord += AD9833_DSS_MAX_PHASE;
  this->writeRegister(freqReg == FREG0 ? round(phaseWord * (4095.0 / AD9833_DSS_MAX_PHASE)) | 0xC000 : round(phaseWord * (4095.0 / AD9833_DSS_MAX_PHASE)) | 0xE000);
  this->m_phase[freqReg == FREG0 ? 0 : 1] = phaseWord;
}
*/

uint32_t AD9833_DDS::getFrequency(channel_t channel) const {
  return channel == FST_CHANNEL ? this->m_freq[0] : this->m_freq[1];
}

signal_type_t AD9833_DDS::getWave(channel_t channel) const {
  return channel == FST_CHANNEL ? this->m_wave[0] : this->m_wave[1];
}

/*
uint16_t AD9833_DDS::getPhase(channel_t channel) const {
  return channel == FST_CHANNEL ? this->m_phase[0] : this->m_phase[1];
}
*/

void AD9833_DDS::configDDS(uint32_t frequency, signal_type_t signalType, uint16_t phase, freqReg_t freqReg) {
  this->setFrequency(frequency, freqReg);
  this->setWave(signalType, freqReg);
  // this->setPhase(phase, freqReg);
}

AD9833_DDS::~AD9833_DDS() { }
