#ifndef _ILI9341_H_
#define _ILI9341_H_

#include "comm.h"
#if (__STM32F1__)
#include <Arduino_ILI9341_STM.h>
#endif

#include "RREFont.h"
// #include "rre_12x16.h"
// #include "rre_bold13x20v.h"

#include "pitches.h"

#define SCR_WD 240
#define SCR_HT 320

#define SYSTEM_FAULT_TEXT_COLOR 0xFF1B // RGB: (255, 226, 220)
#define DEFAULT_BACKGROUND_COLOR 0x18A3 // RGB: (25, 20, 25)
#define FAULT_BACKGROUND_COLOR 0x0020 // RGB: (0, 4, 0)
#define DEFAULT_LETTER_SPACING_SIZE 1

// It's not recommended to modify the following macros
#define MIN_SLOPE_FACTOR_EFFECT 15
#define MAX_SLOPE_FACTOR_EFFECT 39
#define HEIGHT_FLAME_EFFECT 11
#define FLAME_EFFECT_TRANSITION_TIME 47

extern SPISettings ILI9341_SPISettings;
extern Arduino_ILI9341 g_tft;
extern RREFont g_font;

extern RRE_Font rreInstance_12x16;
extern RRE_Font rreInstance_Bold13x20v;

// Needed for RREFont library initialization
void customRect(int x, int y, int w, int h, int c);

void drawImage(const uint16_t *image);
void drawGradientMenu(bool clearBackground = false, uint8_t topOffset = 0, uint16_t bottomOffset = SCR_HT);
void drawGradientFault(const char *error);
// void clearScreenBlock(byte x_initPosition, byte y_initPosition, byte width, byte height); // It would replace by the "fillRect" function.

void configFont(RREFont &font, RRE_Font *fontInstance, uint16_t color, uint8_t scaleX = 1, uint8_t scaleY = 1, uint8_t bold = 1, uint8_t letterSpacing = DEFAULT_LETTER_SPACING_SIZE, uint8_t minWidth = 1, uint16_t backColor = 0, bool backColorUsage = false);

#endif // _ILI9341_H_
