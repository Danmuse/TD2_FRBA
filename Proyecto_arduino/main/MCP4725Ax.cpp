/*/*!
 * @file MCP4725Ax.cpp
 * @par Author & Doxygen Editor
 *  Daniel Di Módica ~ <a href = "mailto: danifabriziodmodica@gmail.com">danifabriziodmodica@@gmail.com</a>
 * @date 20/04/2023 13:36:24
 * @version 1.0
 */

#include "MCP4725Ax.h"

void initMCP(void) {
  Wire.begin();
  Wire.setClock(WIRE_SPEED);
}

analogStatus_t setVoltage(analogMCP_t MCP_address, uint16_t analogValue, flag_t writeEEPROM) {
  if (analogValue > MCP4725_MAX_VALUE_RESOLUTION - 1) return ANALOG_IDLE;
  Wire.beginTransmission(MCP_address);
  if (writeEEPROM) Wire.write(MCP4725_WRITEDAC_EEPROM);
  else Wire.write(MCP4725_WRITEDAC);
  Wire.write(analogValue / 16); // Upper data bits (D11.D10.D9.D8.D7.D6.D5.D4)
  Wire.write((analogValue % 16) << 4); // Lower data bits (D3.D2.D1.D0.x.x.x.x)
  Wire.endTransmission();
  return analogValue ? ANALOG_HIGH : ANALOG_LOW;
  // return (statusI2C_t)Wire.endTransmission();
}
