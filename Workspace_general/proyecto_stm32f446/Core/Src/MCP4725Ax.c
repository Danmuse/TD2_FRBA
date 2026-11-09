#include "MCP4725Ax.h"

static I2C_HandleTypeDef *mcp_i2c;

void initMCP(I2C_HandleTypeDef *hi2c) {
  mcp_i2c = hi2c;   // el I2C ya lo inicializó CubeMX (MX_I2C1_Init)
}

analogStatus_t setVoltage(analogMCP_t MCP_address, uint16_t analogValue, bool writeEEPROM) {
  if (analogValue > MCP4725_MAX_VALUE_RESOLUTION - 1) return ANALOG_IDLE;

  uint8_t buf[3];
  buf[0] = writeEEPROM ? MCP4725_WRITEDAC_EEPROM : MCP4725_WRITEDAC;
  buf[1] = analogValue >> 4;            // D11..D4
  buf[2] = (analogValue & 0x0F) << 4;   // D3..D0 + 4 bits sin uso

  if (HAL_I2C_Master_Transmit(mcp_i2c, (uint16_t)(MCP_address << 1), buf, 3, 100) != HAL_OK)
    return ANALOG_ERROR;

  return analogValue ? ANALOG_HIGH : ANALOG_LOW;
}
