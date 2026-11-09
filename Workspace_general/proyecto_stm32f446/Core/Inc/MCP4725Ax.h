#ifndef MCP4725A_H
#define MCP4725A_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#define MCP4725_WRITEDAC            (0x40)
#define MCP4725_WRITEDAC_EEPROM     (0x60)
#define MCP4725_MAX_VALUE_RESOLUTION 4096

typedef enum {
  ANALOG_LOW  = 0,
  ANALOG_HIGH = 1,
  ANALOG_IDLE = 2,
  ANALOG_ERROR = 3   // nuevo: fallo de bus I2C
} analogStatus_t;

typedef enum {
  analogMCPA0_agnd = 0x60, // A0 pin a GND
  analogMCPA0_avdd = 0x61, // A0 pin a VDD
  analogMCPA1_agnd = 0x62,
  analogMCPA1_avdd = 0x63,
  analogMCPA2_agnd = 0x64,
  analogMCPA2_avdd = 0x65
} analogMCP_t;

void initMCP(I2C_HandleTypeDef *hi2c);
analogStatus_t setVoltage(analogMCP_t MCP_address, uint16_t analogValue, bool writeEEPROM);

#endif
