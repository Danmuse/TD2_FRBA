#include "ILI9341.h"

SPISettings ILI9341_SPISettings(SPI_PERIPHERAL_SPEED, MSBFIRST, SPI_MODE3, DATA_SIZE_16BIT);
Arduino_ILI9341 g_tft(TFT_DC, TFT_RST, TFT_CS);
RREFont g_font;

RRE_Font rreInstance_12x16 = { RRE_16B, 11, 16, 0x20, 0x7f, (const uint8_t*)font12x16_RectsInstance, (const uint16_t*)font12x16_CharOffsInstance };
RRE_Font rreInstance_Bold13x20v = { RRE_V16B, 13, 20, 0x20, 0x3a, (const uint8_t*)fontBold13x20v_RectsInstance, (const uint16_t*)fontBold13x20v_CharOffsInstance };

// Needed for RREFont library initialization
void customRect(int x, int y, int w, int h, int c) { return g_tft.fillRect(x, y, w, h, c); }

void drawImage(const uint16_t *image) {
  for (byte y = 0; y < SCR_HT; y++) {
    for (byte x = 0; x < SCR_WD; x++) {
      uint16_t color = pgm_read_word(&(image[y * SCR_WD + x]));
      // Serial.print("Pixel color: ");
      // Serial.println(color, HEX);
      g_tft.drawPixel(x, y, color);
    }
  }
}

void drawGradientMenu(bool clearBackground) {
  static uint8_t slopeDinamicFactor = MIN_SLOPE_FACTOR_EFFECT;
  static bool incrementSlopeFactor = true;
  static uint32_t flameEffectMillis = 0;
  uint16_t gradientInstance[SCR_WD];
  if (clearBackground) {
    flameEffectMillis = millis();
    g_tft.fillRect(0, 0, SCR_WD, SCR_HT, DEFAULT_BACKGROUND_COLOR);
    slopeDinamicFactor = MIN_SLOPE_FACTOR_EFFECT;
  } else if ((millis() - flameEffectMillis) > FLAME_EFFECT_TRANSITION_TIME) {
    flameEffectMillis = millis();
    if (slopeDinamicFactor == MAX_SLOPE_FACTOR_EFFECT) incrementSlopeFactor = false;
    else if (slopeDinamicFactor == MIN_SLOPE_FACTOR_EFFECT) incrementSlopeFactor = true;
    incrementSlopeFactor ? slopeDinamicFactor++ : slopeDinamicFactor--;
  }
  
  for (byte idx = 0; idx < SCR_WD; idx++) gradientInstance[idx] = pgm_read_word(&gradientHr_ST7789[idx]);
  for (byte lineTFT = 0; lineTFT < HEIGHT_FLAME_EFFECT; lineTFT++) {
    for (byte slopeFactor = 0; slopeFactor < slopeDinamicFactor; slopeFactor++) {
      for (byte idx = 0; idx < SCR_WD / 2; idx++) {
        gradientInstance[(SCR_WD / 2) - 1 + idx] = gradientInstance[(SCR_WD / 2) - 1 + idx + 1];
        gradientInstance[(SCR_WD / 2) - idx] = gradientInstance[(SCR_WD / 2) - idx - 1];
        if (idx != SCR_WD / 2 - 1) gradientInstance[(SCR_WD / 2) - idx - 1] = DEFAULT_BACKGROUND_COLOR;
      }
    }
    g_tft.drawImageF(0, lineTFT + 1, SCR_WD, 1, gradientInstance);
    g_tft.drawImageF(0, SCR_HT - lineTFT - 2, SCR_WD, 1, gradientInstance);
  }
}

void drawGradientFault(const char *error) {
  uint16_t gradientInstance[SCR_WD];
  for (byte idx = 0; idx < SCR_WD; idx++) {
    uint16_t color = pgm_read_word(&gradientHr_ST7789[idx]);
    uint8_t r = (color >> 11) & 0x1F, g = (color >> 5) & 0x3F, b = color & 0x1F;
    gradientInstance[idx] = (b << 11) | (g << 5) | r;
  }
  g_tft.fillRect(0, 0, SCR_WD, SCR_HT, DEFAULT_BACKGROUND_COLOR);
  for (byte lineTFT = 0; lineTFT < HEIGHT_FLAME_EFFECT; lineTFT++) {
    for (byte slopeFactor = 0; slopeFactor < MIN_SLOPE_FACTOR_EFFECT; slopeFactor++) {
      for (byte idx = 0; idx < SCR_WD / 2; idx++) {
        gradientInstance[(SCR_WD / 2) - 1 + idx] = gradientInstance[(SCR_WD / 2) - 1 + idx + 1];
        gradientInstance[(SCR_WD / 2) - idx] = gradientInstance[(SCR_WD / 2) - idx - 1];
        if (idx != SCR_WD / 2 - 1) gradientInstance[(SCR_WD / 2) - idx - 1] = DEFAULT_BACKGROUND_COLOR;
      }
    }
    g_tft.drawImageF(0, lineTFT + 1, SCR_WD, 1, gradientInstance);
    g_tft.drawImageF(0, SCR_HT - lineTFT - 2, SCR_WD, 1, gradientInstance);
  }
  configFont(g_font, &rreInstance_12x16, SYSTEM_FAULT_TEXT_COLOR, 1, 1, 1, DEFAULT_LETTER_SPACING_SIZE, 10, DEFAULT_BACKGROUND_COLOR, true);
  for (byte idx = 0; idx < strlen(error); idx++) {
    if (error[idx] == '\\' && (error[idx+1] == 'n' || error[idx+1] == 'r')) {
      char errorInstance[idx+1];
      strncpy(errorInstance, error, idx);
      errorInstance[idx] = '\0';
      g_font.printStr(ALIGN_CENTER, 100, errorInstance);
      g_font.printStr(ALIGN_CENTER, 123, (char*)&(error[strlen(errorInstance)+2]));
      return;
    }
  }
  g_font.printStr(ALIGN_CENTER, 112, (char*)error);
}

/*
void clearScreenBlock(byte x_initPosition, byte y_initPosition, byte width, byte height) {
  uint16_t cleanVector[width];
  for (byte y = 0; y < height; y++) {
    for (byte x = 0; x < width; x++) {
      cleanVector[x] = 0x0000;
      g_tft.drawImageF(x + x_initPosition, y + y_initPosition, width, 1, cleanVector);
    }
  }
}
*/

void configFont(RREFont &font, RRE_Font *fontInstance, uint16_t color, uint8_t scaleX, uint8_t scaleY, uint8_t bold, uint8_t letterSpacing, uint8_t minWidth, uint16_t backColor, bool backColorUsage) {
  font.setFont(fontInstance);
  backColorUsage ? font.setColor(color, backColor) : font.setColor(color);
  font.setScale(scaleX, scaleY);
  font.setBold(bold);
  font.setSpacing(letterSpacing);
  if (fontInstance->type == RRE_16B) font.setCharMinWd(minWidth < 6 ? 6 : minWidth);
  else if (fontInstance->type == RRE_V16B) font.setDigitMinWd(minWidth < 7 ? 7 : minWidth);
}
