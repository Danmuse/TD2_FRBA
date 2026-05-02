/*!
 * @file AD9833_DDS.h
 * @par Author & Doxygen Editor
 *  Daniel Di Módica ~ <a href = "mailto: danifabriziodmodica@gmail.com">danifabriziodmodica@@gmail.com</a>
 * @date 29/09/2024 13:40:36
 * @version 1.0
 * @brief AD9833 DDS API
 * @details @par Description
 *  The <b>AD9833_DDS</b> library provides an interface for controlling the <b>Analog Devices AD9833</b> Direct Digital Synthesizer (DDS), 
 * a low-power waveform generator capable of producing sine, square, and triangle waves. This library simplifies communication with the AD9833 
 * by providing high-level functions to configure and control its output frequency, phase, and waveform type using a standard SPI interface.
 * @par Features
 *  - Supports setting output frequency for channels FREQ0 and FREQ1
 *  - Allows control over the waveform type: sine, triangle, or square wave
 *  - Control over phase output via PHASE0 and PHASE1 registers
 *  - Easy switching between different frequency/phase registers
 *  - Includes initialization and reset functions for the AD9833 chip
 *  - Low-level SPI communication handled via standard library functions
 * @par Requirements
 *  - SPI communication setup with the microcontroller
 *  - Compatible with 3.3V or 5V logic depending on the microcontroller and AD9833 power supply
 *  - Additional components like decoupling capacitors and a crystal oscillator may be needed for optimal performance
 * @see <a target = "__blank" href = "https://ar.mouser.com/datasheet/2/609/ad9833-3119795.pdf"><strong>AD9833BRMZ</strong></a> Datasheet.
 */

#ifndef _AD9833_DDS_H_
#define _AD9833_DDS_H_

#include "comm.h"

#if defined(ESP8266) || (__STM32F1__)
#define AD9833_DDS_SPI_SPEED  (36000000UL)
#else
#define AD9833_DDS_SPI_SPEED  (8000000UL)
#endif
#define AD9833_DDS_MAX_FREQ   (12500000UL)
#define AD9833_DSS_MAX_PHASE  (360.0)
#define EXTERNAL_CRYSTAL_FREQ (25000000UL)
#define CRYSTAL_FREQ_FACTOR   (268435456.0 / EXTERNAL_CRYSTAL_FREQ)

// CONTROL REGISTER BITS
#define AD9833_B28          (1 << 13)
#define AD9833_HLB          (1 << 12)
#define AD9833_FSELECT      (1 << 11)
#define AD9833_PSELECT      (1 << 10)
#define AD9833_RESET        (1 << 8)
#define AD9833_SLEEP1       (1 << 7)
#define AD9833_SLEEP12      (1 << 6)
#define AD9833_OPBITEN      (1 << 5)
#define AD9833_DIV2         (1 << 3)
#define AD9833_MODE         (1 << 1)

typedef enum {
  SIG_NONE = 0,
  SIG_SINE = 1,
  SIG_FST_SQUARE = 2,
  SIG_SND_SQUARE = 3,
  SIG_TRIANGLE = 4
} signal_type_t;

class AD9833_DDS {
  private:
    enum freqReg_t { FREG0 = 0x4000, FREG1 = 0x8000 };
    enum waveForm_t { WAVE_NONE = (AD9833_B28 | AD9833_SLEEP1 | AD9833_SLEEP12), WAVE_SINE = AD9833_B28, WAVE_SQUARE1 = (AD9833_B28 | AD9833_DIV2 | AD9833_OPBITEN), WAVE_SQUARE2 = (AD9833_B28 | AD9833_OPBITEN), WAVE_TRIANGLE = (AD9833_B28 | AD9833_MODE) };
    
    uint8_t m_selectPin = 0;
    uint32_t m_freq[2] = { 0, 0 };
    signal_type_t m_wave[2] = { SIG_NONE, SIG_NONE };
    uint16_t m_phase[2] = { 0, 0 };

    void reset(void);
    void writeRegister(uint16_t data);
  public: 
    enum channel_t { FST_CHANNEL, SND_CHANNEL };
    AD9833_DDS(uint8_t slaveSelect);
    void begin(void);
    void setFrequency(uint32_t frequency, freqReg_t freqReg = FREG0);
    void setWave(signal_type_t signalType, freqReg_t freqReg = FREG0);
    // void setPhase(uint16_t phase, freqReg_t freqReg = FREG0);
    uint32_t getFrequency(channel_t channel) const;
    signal_type_t getWave(channel_t channel) const;
    // uint16_t getPhase(channel_t channel) const;
    void configDDS(uint32_t frequency, signal_type_t signalType = SIG_SINE, uint16_t phase = 0, freqReg_t freqReg = FREG0);
    virtual ~AD9833_DDS();
};

extern SPISettings AD9833_SPISettings;
extern AD9833_DDS g_signalGenerator;

#endif // _AD9833_DDS_H_
