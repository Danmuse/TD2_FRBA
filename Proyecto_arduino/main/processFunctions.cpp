#include "wirish_time.h"
#include "Arduino_ILI9341_STM.h"
#include "processFunctions.h"

bool flashPlugged = false;
batteryStates_t batteryStatus;

static int32_t getMultiplierFromExponentialNotation(char *exponentialNotation);

static void signal_drawCharts(byte x, uint16_t y, byte w, uint16_t h, char *label, bool hover = false, bool idle = true);
static void signal_displayValue(void *value, byte x, uint16_t y, uint8_t amountOfDecimals, textTypes_t type, bool idle = true);
static void changeSettingSignal(char setting[MAX_SIGNAL_SETTING_SIZE], sig_operation_t operation);

static void updateBatteryStatus(void);
static bool printHeaderGraphics(bool deleteLogo = false);
static void printBatteryStatus(void);

static int32_t getMultiplierFromExponentialNotation(char *exponentialNotation) {
  return (int32_t)pow(10, atoi(&(exponentialNotation[SCIENTIFIC_NOTATION_EXPONENTIAL_POSITION])));
}

static void signal_drawCharts(byte x, uint16_t y, byte w, uint16_t h, char *label, bool hover, bool idle) {
  g_tft.drawRoundRect(x + 0, y + 10, w - 2 * 0, h - 10 - 2 * 0, 10, hover ? FRAME_HOVER_CHARTS_COLOR : FRAME_CHARTS_COLOR(idle));
  g_tft.drawRoundRect(x + 1, y + 11, w - 2 * 1, h - 10 - 2 * 1, 10, hover ? FRAME_HOVER_CHARTS_COLOR : FRAME_CHARTS_COLOR(idle));
  g_tft.drawRoundRect(x + 2, y + 12, w - 2 * 2, h - 10 - 2 * 2, 10, hover ? FRAME_HOVER_CHARTS_COLOR : FRAME_CHARTS_COLOR(idle));
  g_tft.fillRoundRect(x + 3, y + 13, w - 2 * 3, h - 10 - 2 * 3, 10, BACKGROUND_CHARTS_COLOR);
  configFont(g_font, &rreInstance_12x16, hover ? TITLE_HOVER_CHARTS_COLOR : TITLE_CHARTS_COLOR(idle), 1, 1, 1, LETTER_SPACING_SIZE);
  g_font.printStr(x + (w - g_font.strWidth(label)) / 2, y - (g_font.getHeight()) / 2 - LETTER_SPACING_SIZE, label);
}

static void signal_displayValue(void *value, byte x, uint16_t y, uint8_t amountOfDecimals, textTypes_t type, bool idle) {
  char txt[MAX_SIGNAL_SETTING_SIZE];
  configFont(g_font, &rreInstance_Bold13x20v, TEXT_CHARTS_COLOR(idle), 1, 1, 1, LETTER_SPACING_SIZE, 16);
  if (type == TYPE_NONE) return;
  else if (type == TYPE_UINT) citoa(*(uint16_t *)value, txt, 10);
  else if (type == TYPE_INT) citoa(*(int32_t *)value, txt, 10);
  else if (type == TYPE_FLOAT) cftoa(*(float *)value, txt, amountOfDecimals);
  else if (type == TYPE_STRING) {
    configFont(g_font, &rreInstance_12x16, TEXT_CHARTS_COLOR(idle), 1, 2, 2, LETTER_SPACING_SIZE, 13);
    strncpy(txt, (char *)value, strlen((char *)value));
    txt[strlen((char *)value)] = '\0';
    y -= 5;
  } else if (type == TYPE_HIBRID) {
    uint8_t amountOfDigits = 0;
    strncpy(txt, (char *)value, strlen((char *)value));
    txt[strlen((char *)value)] = '\0';
    if (txt[0] == 'x') {
      bool exponentialNotation = false;
      for (byte idx = 1; idx < strlen(txt) - 1 && txt[idx] != '\0'; idx++) {
        if (txt[idx] == '^' && !exponentialNotation && txt[1] == '1' && txt[2] == '0' && idx == SCIENTIFIC_NOTATION_EXPONENTIAL_POSITION - 1) exponentialNotation = true;
        else if (!isNumberExt(txt[idx])) return;
        amountOfDigits++;
      }
      if (exponentialNotation) {
        char unit[2] = { 'x', '\0' };
        char unitBase[3] = { '1', '0', '\0' };
        char *exponentValue = &(txt[SCIENTIFIC_NOTATION_EXPONENTIAL_POSITION]);
        RREFont fontUnits = g_font;
        RREFont fontExponent = g_font;
        configFont(fontUnits, &rreInstance_12x16, TEXT_CHARTS_COLOR(idle), 1, 1, 2, LETTER_SPACING_SIZE, 12);
        configFont(fontExponent, &rreInstance_12x16, TEXT_CHARTS_COLOR(idle), 1, 1, 2, LETTER_SPACING_SIZE, 12);
        fontUnits.printStr(x + (CONTENT_WIDTH_MIDDLE_CHARTS_SIZE - (2 + g_font.strWidth(exponentValue) + g_font.strWidth(unitBase) + fontUnits.strWidth(unit))) / 2 + 4, y + 5, unit);
        g_font.printStr(x + (CONTENT_WIDTH_MIDDLE_CHARTS_SIZE - ((2 + g_font.strWidth(exponentValue) + g_font.strWidth(unitBase)) - fontUnits.strWidth(unit))) / 2 + 4, y - 1, unitBase);
        fontExponent.printStr(x + (CONTENT_WIDTH_MIDDLE_CHARTS_SIZE - (g_font.strWidth(exponentValue) - (2 + g_font.strWidth(unitBase) + fontUnits.strWidth(unit)))) / 2 + 4, y - 1, exponentValue);
      } else {
        if (amountOfDigits > MAX_MULTIPLIER_DIGITS) return;
        char unit[2] = { 'x', '\0' };
        char *multiplierValue = &(txt[1]);
        RREFont fontUnits = g_font;
        configFont(fontUnits, &rreInstance_12x16, TEXT_CHARTS_COLOR(idle), 1, 1, 2, LETTER_SPACING_SIZE, 12);
        fontUnits.printStr(x + (CONTENT_WIDTH_MIDDLE_CHARTS_SIZE - (g_font.strWidth(multiplierValue) + fontUnits.strWidth(unit))) / 2, y + 5, unit);
        g_font.printStr(x + (CONTENT_WIDTH_MIDDLE_CHARTS_SIZE - (g_font.strWidth(multiplierValue) - fontUnits.strWidth(unit))) / 2, y - 1, multiplierValue);
      }
    } else if (txt[strlen(txt) - 2] == 'H' && txt[strlen(txt) - 1] == 'z') {
      for (byte idx = 0; idx < strlen(txt) && txt[idx] != 'H'; idx++) {
        if (!isNumberExt(txt[idx])) return;
        amountOfDigits++;
      }
      if (amountOfDigits > MAX_FREQUENCY_DIGITS) return;
      char *frequencyValue = strtok(txt, "H");
      char unit[4] = { ' ', 'H', 'z', '\0' };
      strncpy(&(txt[strlen(txt)]), "Hz", strlen("Hz"));
      txt[strlen(txt)] = '\0';
      RREFont fontUnits = g_font;
      configFont(fontUnits, &rreInstance_12x16, TEXT_CHARTS_COLOR(idle), 1, 1, 2, LETTER_SPACING_SIZE, 12);
      g_font.printStr(x + (CONTENT_WIDTH_FILL_OUT_CHARTS_SIZE - (fontUnits.strWidth(unit) + g_font.strWidth(frequencyValue))) / 2 + 3, y - 1, frequencyValue);
      fontUnits.printStr(x + (CONTENT_WIDTH_FILL_OUT_CHARTS_SIZE - (fontUnits.strWidth(unit) - g_font.strWidth(frequencyValue))) / 2 - 3, y + 5, unit);
    }
    return;
  }
  g_font.printStr(x + (CONTENT_WIDTH_MIDDLE_CHARTS_SIZE - g_font.strWidth(txt)) / 2, y, txt);
}

static void changeSettingSignal(char setting[MAX_SIGNAL_SETTING_SIZE], sig_operation_t operation) {
#ifndef MULTIPLIER_EXPONENTIAL_NOTATION_REPRESENTATION
  const char *multipliers[] = { "1", "10", "100", "1000", "10000" };
#else
  const char *exponents[] = { "10^0", "10^1", "10^2", "10^3", "10^4", "10^5" };
#endif  // !defined(MULTIPLIER_EXPONENTIAL_NOTATION_REPRESENTATION)
  const char *signalTypes[] = { "SINE", "SQUARE", "TRIANG" };
  const uint8_t numSignalTypes = sizeof(signalTypes) / sizeof(signalTypes[0]);
  static uint32_t multiplierStored = 1000;
  uint16_t blinkInterval = ACTIVE_BLINK_DIGIT_TIME;
  uint32_t blinkTimeElapsed = 0;
  bool blinkStatus = true;
  g_encoder.mismatchEngaged();
#if DEFAULT_TOP_OFFSET_IMAGES != 0
  g_tft.fillRect(0, HEADER_PANEL_ELEMENTS_TOP_OFFSET, LOGO_IMAGE_WIDTH, LOGO_IMAGE_HEIGHT / 2, HEADER_PANEL_COLOR);
  configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_COLOR(false));
  g_font.printStr(5, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO CONFIRM");
#else
  g_tft.fillRect(0, DEFAULT_TOP_OFFSET_IMAGES, SCR_WD, HEADER_PANEL_ELEMENTS_FRAME_HEIGHT, BACKGROUND_CHARTS_COLOR);
  g_tft.drawFastHLine(0, DEFAULT_TOP_OFFSET_IMAGES + HEADER_PANEL_ELEMENTS_FRAME_HEIGHT, SCR_WD, FRAME_PANEL_ELEMENTS_COLOR);
  g_tft.drawFastHLine(0, DEFAULT_TOP_OFFSET_IMAGES + HEADER_PANEL_ELEMENTS_FRAME_HEIGHT + 1, SCR_WD, FRAME_PANEL_ELEMENTS_COLOR);
  g_tft.drawFastHLine(0, DEFAULT_TOP_OFFSET_IMAGES + HEADER_PANEL_ELEMENTS_FRAME_HEIGHT + 2, SCR_WD, FRAME_PANEL_ELEMENTS_COLOR);
  configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_COLOR(false), 1, 2, 2, 2, 10);
  g_font.printStr(ALIGN_CENTER, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO CONFIRM");
#endif

  if (operation == SIG_OP_SET_TYPE) {
    g_encoder.changeRange(ENCODER_LOWER_LIMIT_SIGNAL_SIG_TYPE_OPTIONS, ENCODER_UPPER_LIMIT_SIGNAL_SIG_TYPE_OPTIONS);
    for (byte idx = 0; idx < numSignalTypes; idx++) {
      if (!strcmp(&(setting[0]), signalTypes[idx])) {
        g_encoder.setPosition(idx);
        break;
      }
    }
    uint8_t currentPosition = g_encoder.getPosition();
    blinkTimeElapsed = millis();
    while (!g_encoder.buttonPressed()) {
      g_encoder.rotaryEncoderCallback();
      if (currentPosition != g_encoder.getPosition()) {
        currentPosition = g_encoder.getPosition();
        g_encoder.mismatchEngaged();
        snprintf(setting, MAX_SIGNAL_SETTING_SIZE, "%s", signalTypes[currentPosition]);
        signal_drawCharts(MARGIN_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "SIG. TYPE", true);
        signal_displayValue(setting, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_STRING, true);
      }
      blinkInterval = blinkStatus ? ACTIVE_BLINK_DIGIT_TIME : INACTIVE_BLINK_DIGIT_TIME;
      if (millis() - blinkTimeElapsed > blinkInterval) {
        blinkTimeElapsed = millis();
        blinkStatus = !blinkStatus;
        g_encoder.mismatchEngaged();
        signal_drawCharts(MARGIN_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "SIG. TYPE", true);
        if (blinkStatus) signal_displayValue(setting, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_STRING, true);
        else {
          char settingInstance[MAX_SIGNAL_SETTING_SIZE];
          memset(settingInstance, ' ', strlen(signalTypes[currentPosition]));
          settingInstance[strlen(signalTypes[currentPosition])] = '\0';
          signal_displayValue(settingInstance, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_STRING, true);
        }
      }
    }
  } else if (operation == SIG_OP_SET_MULT) {
    g_encoder.changeRange(ENCODER_LOWER_LIMIT_SIGNAL_MULTIPLIER_OPTIONS, ENCODER_UPPER_LIMIT_SIGNAL_MULTIPLIER_OPTIONS);
#ifndef MULTIPLIER_EXPONENTIAL_NOTATION_REPRESENTATION
    for (byte idx = 0; idx < ENCODER_UPPER_LIMIT_SIGNAL_MULTIPLIER_OPTIONS + 1; idx++) {
      if (!strcmp(&(setting[1]), multipliers[idx])) {
        g_encoder.setPosition(idx);
        break;
      }
    }
    uint8_t currentPosition = g_encoder.getPosition();
    blinkTimeElapsed = millis();
    while (!g_encoder.buttonPressed()) {
      g_encoder.rotaryEncoderCallback();
      if (currentPosition != g_encoder.getPosition()) {
        currentPosition = g_encoder.getPosition();
        g_encoder.mismatchEngaged();
        multiplierStored = atoi(multipliers[currentPosition]);
        snprintf(setting, MAX_SIGNAL_SETTING_SIZE, "x%s", multipliers[currentPosition]);
        signal_drawCharts(MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "MULTIPLIER", true);
        signal_displayValue(setting, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_HIBRID, true);
      }
      blinkInterval = blinkStatus ? ACTIVE_BLINK_DIGIT_TIME : INACTIVE_BLINK_DIGIT_TIME;
      if (millis() - blinkTimeElapsed > blinkInterval) {
        blinkTimeElapsed = millis();
        blinkStatus = !blinkStatus;
        g_encoder.mismatchEngaged();
        signal_drawCharts(MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "MULTIPLIER", true);
        if (blinkStatus) signal_displayValue(setting, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_HIBRID, true);
        else {
          char settingInstance[MAX_SIGNAL_SETTING_SIZE] = "x";
          memset(settingInstance + 1, ' ', strlen(multipliers[currentPosition]));
          settingInstance[strlen(multipliers[currentPosition]) + 1] = '\0';
          signal_displayValue(settingInstance, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_HIBRID, true);
        }
      }
    }
#else
    for (byte idx = 0; idx < ENCODER_UPPER_LIMIT_SIGNAL_MULTIPLIER_OPTIONS + 1; idx++) {
      if (!strcmp(&(setting[1]), exponents[idx])) {
        g_encoder.setPosition(idx);
        break;
      }
    }
    uint8_t currentPosition = g_encoder.getPosition();
    while (!g_encoder.buttonPressed()) {
      g_encoder.rotaryEncoderCallback();
      if (currentPosition != g_encoder.getPosition()) {
        currentPosition = g_encoder.getPosition();
        g_encoder.mismatchEngaged();
        snprintf(setting, MAX_SIGNAL_SETTING_SIZE, "x%s", exponents[currentPosition]);
        multiplierStored = getMultiplierFromExponentialNotation(setting);
        signal_drawCharts(MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "MULTIPLIER", true);
        signal_displayValue(setting, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_HIBRID, true);
      }
      blinkInterval = blinkStatus ? ACTIVE_BLINK_DIGIT_TIME : INACTIVE_BLINK_DIGIT_TIME;
      if (millis() - blinkTimeElapsed > blinkInterval) {
        blinkTimeElapsed = millis();
        blinkStatus = !blinkStatus;
        g_encoder.mismatchEngaged();
        signal_drawCharts(MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "MULTIPLIER", true);
        if (blinkStatus) signal_displayValue(setting, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_HIBRID, true);
        else {
          char settingInstance[MAX_SIGNAL_SETTING_SIZE] = "x10";
          memset(settingInstance + (SCIENTIFIC_NOTATION_EXPONENTIAL_POSITION - 1), ' ', strlen(exponents[currentPosition]) - SCIENTIFIC_NOTATION_EXPONENTIAL_POSITION);
          settingInstance[strlen(exponents[currentPosition]) - SCIENTIFIC_NOTATION_EXPONENTIAL_POSITION + (SCIENTIFIC_NOTATION_EXPONENTIAL_POSITION - 1)] = '\0';
          signal_displayValue(settingInstance, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_LEFT_CENTER_MIDDLE_CHARTS - 6, VERTICAL_PADDING_CHARTS(2), 0, TYPE_HIBRID, true);
        }
      }
    }
#endif  // !defined(MULTIPLIER_EXPONENTIAL_NOTATION_REPRESENTATION)
  } else if (operation == SIG_OP_SET_FREQ) {
    g_encoder.changeRange(ENCODER_LOWER_LIMIT_SIGNAL_FREQUENCY, ENCODER_UPPER_LIMIT_SIGNAL_FREQUENCY);
    char *frequencyValue = strtok(setting, "H");
    strncpy(&(setting[strlen(setting)]), "Hz", strlen("Hz"));
    setting[strlen(setting)] = '\0';
    g_encoder.setPosition(atoi(frequencyValue));
    g_encoder.setMultiplier(multiplierStored);
    uint32_t currentPosition = g_encoder.getPosition();
    blinkTimeElapsed = millis();
    while (!g_encoder.buttonPressed()) {
      g_encoder.rotaryEncoderCallback();
      if (currentPosition != g_encoder.getPosition()) {
        currentPosition = g_encoder.getPosition();
        g_encoder.mismatchEngaged();
        snprintf(setting, MAX_SIGNAL_SETTING_SIZE, "%uHz", currentPosition);
        signal_drawCharts(MARGIN_FILL_OUT_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(3), WIDTH_FILL_OUT_CHARTS, HEIGHT_CHARTS, "FREQUENCY", true);
        signal_displayValue(setting, HORIZONTAL_PADDING_FILL_OUT_CHARTS, VERTICAL_PADDING_CHARTS(3), 0, TYPE_HIBRID, true);
      }
      blinkInterval = blinkStatus ? ACTIVE_BLINK_DIGIT_TIME : INACTIVE_BLINK_DIGIT_TIME;
      if (millis() - blinkTimeElapsed > blinkInterval) {
        blinkTimeElapsed = millis();
        blinkStatus = !blinkStatus;
        g_encoder.mismatchEngaged();
        signal_drawCharts(MARGIN_FILL_OUT_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(3), WIDTH_FILL_OUT_CHARTS, HEIGHT_CHARTS, "FREQUENCY", true);
        if (blinkStatus) signal_displayValue(setting, HORIZONTAL_PADDING_FILL_OUT_CHARTS, VERTICAL_PADDING_CHARTS(3), 0, TYPE_HIBRID, true);
        else {
          char settingInstance[MAX_SIGNAL_SETTING_SIZE];
          memset(settingInstance, ' ', strlen(setting) - strlen("Hz"));
          strncpy(&(settingInstance[strlen(setting) - strlen("Hz")]), "Hz", strlen("Hz"));
          settingInstance[strlen(setting)] = '\0';
          signal_displayValue(settingInstance, HORIZONTAL_PADDING_FILL_OUT_CHARTS, VERTICAL_PADDING_CHARTS(3), 0, TYPE_HIBRID, true);
        }
      }
    }
    g_encoder.usageMultiplier(false);
  }
}

static void updateBatteryStatus(void) {
  static uint32_t timeElapsed = millis();
  if (millis() - timeElapsed > 5000) {
    // ---------- TODO: CHANGE THE FOLLOWING INSTRUCTIONS ----------
    timeElapsed = millis();
    batteryStatus = (batteryStates_t)((batteryStatus + 1) % 5);
    // -------------------------------------------------------------
  }
}

static bool printHeaderGraphics(bool deleteLogo) {
  // It is recommended not to modify the "BUFFER_IDX_FRAGMENT" macro so as not to slow down or affect the representation of the image.
  // If the value of "BUFFER_IDX_FRAGMENT" is 6, 14400 bytes will be stored, that is, 72% of the SRAM memory occupied for the STM32F103x family of microcontrollers.

  if (!flashPlugged) {
    drawGradientMenu(true);
    return false;
  }

  drawGradientMenu(false, FRAME_PANEL_ELEMENTS_HEIGHT, DEFAULT_TOP_OFFSET_IMAGES - FRAME_PANEL_ELEMENTS_HEIGHT);
  uint16_t bufferRead[(QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT];
  for (uint8_t bufferIdxFragment = HEADER_IMAGE_SECTOR_FRAGMENT; bufferIdxFragment < HEADER_IMAGE_SECTOR_FRAGMENT + 4; bufferIdxFragment++) {
    spiTransaction(W25Q64xx_SPISettings, []() {});
    W25Qxx_result_t statusRead = g_flash.read(bufferRead, W25Q64xx::UINT16, QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT, ((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * bufferIdxFragment + 256) % W25QXX_BYTES_PER_PAGE, ((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * bufferIdxFragment + 256) / W25QXX_BYTES_PER_PAGE + MAX_PAGE_TO_PROCESS_ANY_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
    spiTransaction(ILI9341_SPISettings, []() {});
    if (statusRead != W25QXX_OK) {
      uint32_t timeElapsed = millis();
      drawGradientFault(ERR_FLASH_READ_FAILURE);
      while (true)
        if (millis() - timeElapsed > 2000) break;
      drawGradientMenu(true);
      return false;
    }
    g_tft.drawImage(0, TOTAL_HEADER_IMAGE_HEIGHT_POSITION + (bufferIdxFragment - HEADER_IMAGE_SECTOR_FRAGMENT) * (((QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT) / SCR_WD /*SCR_HT*/), SCR_WD, ((QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT) / SCR_WD /*SCR_HT*/, bufferRead);
  }
  if (deleteLogo) g_tft.fillRect(0, sizeof(gradientHBackgroundHeader_ST7789) / BYTES_SIZE_TO_TYPE_16 + FRAME_PANEL_ELEMENTS_HEIGHT + HEIGHT_FLAME_EFFECT, LOGO_IMAGE_WIDTH, LOGO_IMAGE_HEIGHT, HEADER_PANEL_COLOR);
  for (uint8_t idx = 0; idx < sizeof(gradientHBackgroundHeader_ST7789) / BYTES_SIZE_TO_TYPE_16; idx++) g_tft.drawFastHLine(0, FRAME_PANEL_ELEMENTS_HEIGHT + HEIGHT_FLAME_EFFECT + idx, SCR_WD, gradientHBackgroundHeader_ST7789[idx]);
  for (uint8_t idx = sizeof(gradientHBackgroundHeader_ST7789) / BYTES_SIZE_TO_TYPE_16; idx; idx--) g_tft.drawFastHLine(0, DEFAULT_TOP_OFFSET_IMAGES - (FRAME_PANEL_ELEMENTS_HEIGHT + HEIGHT_FLAME_EFFECT + idx - 1), SCR_WD, gradientHBackgroundHeader_ST7789[idx - 1]);
  g_tft.fillRect(0, 0, SCR_WD, FRAME_PANEL_ELEMENTS_HEIGHT, FRAME_PANEL_ELEMENTS_COLOR);
  g_tft.fillRect(0, DEFAULT_TOP_OFFSET_IMAGES - FRAME_PANEL_ELEMENTS_HEIGHT, SCR_WD, FRAME_PANEL_ELEMENTS_HEIGHT, FRAME_PANEL_ELEMENTS_COLOR);
  return true;
}

static void printBatteryStatus(void) {
  if (!flashPlugged) return;
  static batteryStates_t batteryLastStatus = (batteryStatus == BATTERY_CHARGING ? BATTERY_FULL : BATTERY_CHARGING);
  if (batteryLastStatus != batteryStatus) {
    batteryLastStatus = batteryStatus;
    uint16_t bufferRead[BATTERY_STATES_FRAGMENT_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16];
    uint16_t bufferBatteryStatus[BATTERY_STATES_FRAGMENT_WIDTH * BATTERY_STATES_FRAGMENT_HEIGHT];
    spiTransaction(W25Q64xx_SPISettings, []() {});
    W25Qxx_result_t statusRead = g_flash.read(bufferRead, W25Q64xx::UINT16, BATTERY_STATES_FRAGMENT_BYTES_LENGTH, ((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * BATTERY_STATES_FRAGMENT_SECTOR_FRAGMENT + BATTERY_STATES_FRAGMENT_OFFSET) % W25QXX_BYTES_PER_PAGE, ((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * BATTERY_STATES_FRAGMENT_SECTOR_FRAGMENT + BATTERY_STATES_FRAGMENT_OFFSET) / W25QXX_BYTES_PER_PAGE + MAX_PAGE_TO_PROCESS_ANY_IMAGE * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
    spiTransaction(ILI9341_SPISettings, []() {});
    if (statusRead != W25QXX_OK) return;
    for (uint8_t idx = 0; idx < BATTERY_STATES_FRAGMENT_HEIGHT; idx++) {
      for (uint8_t jdx = 0; jdx < BATTERY_STATES_FRAGMENT_WIDTH; jdx++) {
        bufferBatteryStatus[jdx + idx * BATTERY_STATES_FRAGMENT_WIDTH] = bufferRead[jdx + idx * BATTERY_STATES_FRAGMENT_WIDTH + batteryStatus * (BATTERY_STATES_FRAGMENT_WIDTH * BATTERY_STATES_FRAGMENT_HEIGHT)];
      }
    }
    g_tft.drawImage(201, BATTERY_STATES_FRAGMENT_HEIGHT + TOTAL_HEADER_IMAGE_HEIGHT_POSITION, BATTERY_STATES_FRAGMENT_WIDTH, BATTERY_STATES_FRAGMENT_HEIGHT, bufferBatteryStatus);
  }
}

void printFromFlash(flashImages_t numImage, uint16_t topOffset) {
  // It is recommended not to modify the "BUFFER_IDX_FRAGMENT" macro so as not to slow down or affect the representation of the image.
  // If the value of "BUFFER_IDX_FRAGMENT" is 6, 14400 bytes will be stored, that is, 72% of the SRAM memory occupied for the STM32F103x family of microcontrollers.

  if (!flashPlugged) {
    uint8_t attempts = 0;
    spiTransaction(W25Q64xx_SPISettings, []() {});
    while (attempts < FLASH_CONNECTION_ATTEMPTS) {
      if (!(g_flash.begin())) {
        flashPlugged = true;
        break;
      }
      attempts++;
    }
    spiTransaction(ILI9341_SPISettings, []() {});
    if (!flashPlugged) {
      uint32_t timeElapsed = millis();
      drawGradientFault(ERR_FLASH_NOT_FOUND);
      while (true)
        if (millis() - timeElapsed > 2000) break;
      return;
    }
  }

  // uint32_t timeElapsed = millis();
  if (numImage > MAX_PAGE_TO_PROCESS_ANY_IMAGE) return;
  g_tft.fillRect(0, topOffset, SCR_WD, SCR_HT, DEFAULT_BACKGROUND_COLOR);
  uint16_t bufferRead[(QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT];
  uint8_t bufferIdxFragmentInstance = 0;

  for (uint8_t bufferIdxFragment = 0; bufferIdxFragment < TOTAL_FRAGMENTS_TO_PROCESS_ENTIRE_IMAGE; bufferIdxFragment++) {
    bufferIdxFragmentInstance = bufferIdxFragment; /* (bufferIdxFragment < TOTAL_FRAGMENTS_TO_PROCESS_ENTIRE_IMAGE / 2 ? bufferIdxFragment * 2 : (bufferIdxFragment - TOTAL_FRAGMENTS_TO_PROCESS_ENTIRE_IMAGE / 2) * 2 + 1); */
    spiTransaction(W25Q64xx_SPISettings, []() {});
    W25Qxx_result_t statusRead = g_flash.read(bufferRead, W25Q64xx::UINT16, QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT, ((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * bufferIdxFragmentInstance) % W25QXX_BYTES_PER_PAGE, ((QUARTER_IMAGE_BYTES_LENGTH / BUFFER_IDX_FRAGMENT) * bufferIdxFragmentInstance) / W25QXX_BYTES_PER_PAGE + numImage * (TOTAL_SECTORS_TO_PROCESS_ENTIRE_IMAGE * 16));
    spiTransaction(ILI9341_SPISettings, []() {});
    if (statusRead != W25QXX_OK) {
      uint32_t timeElapsed = millis();
      drawGradientFault(ERR_FLASH_READ_FAILURE);
      while (true)
        if (millis() - timeElapsed > 2000) break;
      return;
    }
    g_tft.drawImage(0, topOffset + bufferIdxFragmentInstance * (((QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT) / SCR_WD /*SCR_HT*/), SCR_WD, ((QUARTER_IMAGE_BYTES_LENGTH / BYTES_SIZE_TO_TYPE_16) / BUFFER_IDX_FRAGMENT) / SCR_WD /*SCR_HT*/, bufferRead);
  }
  // uint32_t result = millis() - timeElapsed;
  // Serial.print("Time elapsed: ");
  // Serial.println(result);
}

void displayMenu(operation_t &operation) {
  static bool alreadyDrawFrame = false;
  static bool DrewFrameWithSuccess = false;
  static operation_t currentOperation = OP_NONE;
  g_encoder.rotaryEncoderCallback();
  if (!alreadyDrawFrame) {
    g_encoder.mismatchEngaged();
#if DEFAULT_TOP_OFFSET_IMAGES != 0
    DrewFrameWithSuccess = printHeaderGraphics();
#endif
    printFromFlash(MFRAME_ENTIRE_IMG);
    if (!flashPlugged) g_tft.fillRect(0, 0, SCR_WD, SCR_HT, DEFAULT_BACKGROUND_COLOR);
    alreadyDrawFrame = true;
  }
#if DEFAULT_TOP_OFFSET_IMAGES != 0
  if (DrewFrameWithSuccess) {
    drawGradientMenu(false, FRAME_PANEL_ELEMENTS_HEIGHT, DEFAULT_TOP_OFFSET_IMAGES - FRAME_PANEL_ELEMENTS_HEIGHT);
    updateBatteryStatus();
    printBatteryStatus();
  } else drawGradientMenu();
#endif
  if (currentOperation != g_encoder.getPosition()) {
    currentOperation = (operation_t)g_encoder.getPosition();
    g_tft.fillRoundRect(48, DrewFrameWithSuccess ? 88 + FRAME_GRAPH_OFFSET + ((TOTAL_HEADER_IMAGE_HEIGHT + TOTAL_HEADER_IMAGE_HEIGHT_POSITION + 1) / 2) : 88 + FRAME_GRAPH_OFFSET, 145, 50, 10, DrewFrameWithSuccess ? MENU_FRAME_BACKGROUND_COLOR : BACKGROUND_COLOR_MENU);
    configFont(g_font, &rreInstance_12x16, TEXT_COLOR_MENU, 1, 1, 1, LETTER_SPACING_SIZE, 12, BACKGROUND_COLOR_MENU, true);
    switch (currentOperation) {
      case OP_CALCULATE_INDUCTANCE:
        g_font.printStr(ALIGN_CENTER, DrewFrameWithSuccess ? 95 + FRAME_GRAPH_OFFSET + ((TOTAL_HEADER_IMAGE_HEIGHT + TOTAL_HEADER_IMAGE_HEIGHT_POSITION + 1) / 2) : 95 + FRAME_GRAPH_OFFSET, "INDUCTANCE");
        g_font.printStr(ALIGN_CENTER, DrewFrameWithSuccess ? 118 + FRAME_GRAPH_OFFSET + ((TOTAL_HEADER_IMAGE_HEIGHT + TOTAL_HEADER_IMAGE_HEIGHT_POSITION + 1) / 2) : 118 + FRAME_GRAPH_OFFSET, "METER");
        break;
      case OP_CALCULATE_QUALITY_FACTOR:
        g_font.printStr(ALIGN_CENTER, DrewFrameWithSuccess ? 95 + FRAME_GRAPH_OFFSET + ((TOTAL_HEADER_IMAGE_HEIGHT + TOTAL_HEADER_IMAGE_HEIGHT_POSITION + 1) / 2) : 95 + FRAME_GRAPH_OFFSET, "QUALITY");
        g_font.printStr(ALIGN_CENTER, DrewFrameWithSuccess ? 118 + FRAME_GRAPH_OFFSET + ((TOTAL_HEADER_IMAGE_HEIGHT + TOTAL_HEADER_IMAGE_HEIGHT_POSITION + 1) / 2) : 118 + FRAME_GRAPH_OFFSET, "METER");
        break;
      case OP_GENERATE_SIGNALS:
        g_font.printStr(ALIGN_CENTER, DrewFrameWithSuccess ? 95 + FRAME_GRAPH_OFFSET + ((TOTAL_HEADER_IMAGE_HEIGHT + TOTAL_HEADER_IMAGE_HEIGHT_POSITION + 1) / 2) : 95 + FRAME_GRAPH_OFFSET, "SIGNAL");
        g_font.printStr(ALIGN_CENTER, DrewFrameWithSuccess ? 118 + FRAME_GRAPH_OFFSET + ((TOTAL_HEADER_IMAGE_HEIGHT + TOTAL_HEADER_IMAGE_HEIGHT_POSITION + 1) / 2) : 118 + FRAME_GRAPH_OFFSET, "GENERATOR");
        break;
      case OP_NONE:
      default:
        break;
    }
  }
  if (g_encoder.buttonPressed()) {
    operation = currentOperation;
    currentOperation = OP_NONE;
    alreadyDrawFrame = false;
  }
}

void processOperation(operation_t &operation) {
  switch (operation) {
    case OP_CALCULATE_INDUCTANCE:
      g_encoder.mismatchEngaged();
      printFromFlash(INDUC_IMG);
      if (flashPlugged) {
        uint32_t timeElapsed = millis();
        while (true) {
#if DEFAULT_TOP_OFFSET_IMAGES != 0
          drawGradientMenu(false, FRAME_PANEL_ELEMENTS_HEIGHT, DEFAULT_TOP_OFFSET_IMAGES - FRAME_PANEL_ELEMENTS_HEIGHT);
          printBatteryStatus();
#endif
          if (millis() - timeElapsed > 2000) break;
        }
        printFromFlash(MBACK_IMG);
      }

      if (!flashPlugged) drawGradientFault(ERR_SECTION_NOT_AVAILABLE);  // TODO: Change this instruction. Clean the TFT.
      while (!g_encoder.buttonPressed()) {
        // TODO: It's not implemented yet
      }

      g_encoder.changeRange(ENCODER_LOWER_LIMIT_MENU, ENCODER_UPPER_LIMIT_MENU);
      g_encoder.setPosition(OP_CALCULATE_INDUCTANCE);
      operation = OP_NONE;
      break;
    case OP_CALCULATE_QUALITY_FACTOR:
      g_encoder.mismatchEngaged();
      printFromFlash(QMETER_IMG);
      if (flashPlugged) {
        uint32_t timeElapsed = millis();
        while (true) {
#if DEFAULT_TOP_OFFSET_IMAGES != 0
          drawGradientMenu(false, FRAME_PANEL_ELEMENTS_HEIGHT, DEFAULT_TOP_OFFSET_IMAGES - FRAME_PANEL_ELEMENTS_HEIGHT);
          printBatteryStatus();
#endif
          if (millis() - timeElapsed > 2000) break;
        }
        printFromFlash(MBACK_IMG);
      }

      if (!flashPlugged) drawGradientFault(ERR_SECTION_NOT_AVAILABLE);  // TODO: Change this instruction. Clean the TFT.
      while (!g_encoder.buttonPressed()) {
        // TODO: It's not implemented yet
      }

      g_encoder.changeRange(ENCODER_LOWER_LIMIT_MENU, ENCODER_UPPER_LIMIT_MENU);
      g_encoder.setPosition(OP_CALCULATE_QUALITY_FACTOR);
      operation = OP_NONE;
      break;
    case OP_GENERATE_SIGNALS:
      g_encoder.mismatchEngaged();
      printFromFlash(SIG_IMG);
      if (flashPlugged) {
        uint32_t timeElapsed = millis();
        while (true) {
#if DEFAULT_TOP_OFFSET_IMAGES != 0
          drawGradientMenu(false, FRAME_PANEL_ELEMENTS_HEIGHT, DEFAULT_TOP_OFFSET_IMAGES - FRAME_PANEL_ELEMENTS_HEIGHT);
          printBatteryStatus();
#endif
          if (millis() - timeElapsed > 2000) break;
        }
        printFromFlash(MBACK_IMG);
      }

      if (!flashPlugged) g_tft.clearScreen();
      settingSignal();
      g_encoder.changeRange(ENCODER_LOWER_LIMIT_MENU, ENCODER_UPPER_LIMIT_MENU);
      g_encoder.setPosition(OP_GENERATE_SIGNALS);
      operation = OP_NONE;
      break;
    case OP_NONE:
    default:
      break;
  }
}

void settingSignal(void) {
  g_encoder.changeRange(ENCODER_LOWER_LIMIT_SIGNAL_PANEL, ENCODER_UPPER_LIMIT_SIGNAL_PANEL);
  g_encoder.setPosition(SIG_OP_COMPLETE);
  while (true) {
    static bool alreadyDrawPanel = false;
    static bool DrewFrameWithSuccess = false;
    static sig_operation_t signalOperation = SIG_OP_CANCEL;
    static char strMultiplier[MAX_SIGNAL_SETTING_SIZE];
    static char strSignalType[MAX_SIGNAL_SETTING_SIZE];
    static char strFrequency[MAX_SIGNAL_SETTING_SIZE];
    g_encoder.rotaryEncoderCallback();
    if (!alreadyDrawPanel) {
      g_encoder.mismatchEngaged();
#if DEFAULT_TOP_OFFSET_IMAGES != 0
      DrewFrameWithSuccess = printHeaderGraphics(true);
#endif
      if (*strMultiplier == '\0' || *strSignalType == '\0' || *strFrequency == '\0') {
#ifndef MULTIPLIER_EXPONENTIAL_NOTATION_REPRESENTATION
        strMultiplier[0] = 'x';
        citoa(DEFAULT_SIGNAL_MULTIPLIER, &(strMultiplier[1]), 10);
#else
        strncpy(strMultiplier, "x10^", strlen("x10^"));
        citoa(DEFAULT_SIGNAL_MULTIPLIER, &(strMultiplier[SCIENTIFIC_NOTATION_EXPONENTIAL_POSITION]), 10);
#endif  // !defined(MULTIPLIER_EXPONENTIAL_NOTATION_REPRESENTATION)
        if (DEFAULT_SIGNAL_TYPE == SIG_SINE) {
          strncpy(strSignalType, "SINE", strlen("SINE"));
          strSignalType[strlen("SINE")] = '\0';
        } else if (DEFAULT_SIGNAL_TYPE == SIG_FST_SQUARE) {
          strncpy(strSignalType, "SQUARE", strlen("SQUARE"));
          strSignalType[strlen("SQUARE")] = '\0';
        } else if (DEFAULT_SIGNAL_TYPE == SIG_TRIANGLE) {
          strncpy(strSignalType, "TRIANG", strlen("TRIANG"));
          strSignalType[strlen("TRIANG")] = '\0';
        }
        citoa(DEFAULT_SIGNAL_FREQUENCY, &(strFrequency[0]), 10);
        strncpy(&(strFrequency[strlen(strFrequency)]), "Hz", strlen("Hz"));
        strFrequency[strlen(strFrequency)] = '\0';
        signal_displayValue(strSignalType, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_STRING);
        signal_displayValue(strMultiplier, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_HIBRID);
        signal_displayValue(strFrequency, HORIZONTAL_PADDING_FILL_OUT_CHARTS, VERTICAL_PADDING_CHARTS(3), 0, TYPE_HIBRID);
      }
      alreadyDrawPanel = true;
    }
#if DEFAULT_TOP_OFFSET_IMAGES != 0
    if (DrewFrameWithSuccess) {
      drawGradientMenu(false, FRAME_PANEL_ELEMENTS_HEIGHT, DEFAULT_TOP_OFFSET_IMAGES - FRAME_PANEL_ELEMENTS_HEIGHT);
      updateBatteryStatus();
      printBatteryStatus();
    }
#endif
    if (signalOperation != g_encoder.getPosition()) {
      g_encoder.mismatchEngaged();
      signalOperation = (sig_operation_t)g_encoder.getPosition();
      signal_drawCharts(MARGIN_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(1), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "AMPLITUDE");
      signal_drawCharts(MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(1), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "OFFSET");
      signal_drawCharts(MARGIN_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "SIG. TYPE");
      signal_drawCharts(MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "MULTIPLIER");
      signal_drawCharts(MARGIN_FILL_OUT_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(3), WIDTH_FILL_OUT_CHARTS, HEIGHT_CHARTS, "FREQUENCY");
      signal_displayValue(strSignalType, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_STRING);
      signal_displayValue(strMultiplier, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_HIBRID);
      signal_displayValue(strFrequency, HORIZONTAL_PADDING_FILL_OUT_CHARTS, VERTICAL_PADDING_CHARTS(3), 0, TYPE_HIBRID);
#if DEFAULT_TOP_OFFSET_IMAGES != 0
      g_tft.fillRect(0, HEADER_PANEL_ELEMENTS_TOP_OFFSET, LOGO_IMAGE_WIDTH, LOGO_IMAGE_HEIGHT / 2, HEADER_PANEL_COLOR);
#else
      g_tft.fillRect(0, DEFAULT_TOP_OFFSET_IMAGES, SCR_WD, HEADER_PANEL_ELEMENTS_FRAME_HEIGHT, BACKGROUND_CHARTS_COLOR);
      g_tft.drawFastHLine(0, DEFAULT_TOP_OFFSET_IMAGES + HEADER_PANEL_ELEMENTS_FRAME_HEIGHT, SCR_WD, FRAME_PANEL_ELEMENTS_COLOR);
      g_tft.drawFastHLine(0, DEFAULT_TOP_OFFSET_IMAGES + HEADER_PANEL_ELEMENTS_FRAME_HEIGHT + 1, SCR_WD, FRAME_PANEL_ELEMENTS_COLOR);
      g_tft.drawFastHLine(0, DEFAULT_TOP_OFFSET_IMAGES + HEADER_PANEL_ELEMENTS_FRAME_HEIGHT + 2, SCR_WD, FRAME_PANEL_ELEMENTS_COLOR);
#endif
      switch (signalOperation) {
        case SIG_OP_CANCEL:
#if DEFAULT_TOP_OFFSET_IMAGES != 0
          configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_INACTIVE_COLOR);
          g_font.printStr(24, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO EXIT");
#else
          configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_INACTIVE_COLOR, 1, 2, 2, 2, 10);
          g_font.printStr(ALIGN_CENTER, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO EXIT");
#endif
          break;
        case SIG_OP_COMPLETE:
#if DEFAULT_TOP_OFFSET_IMAGES != 0
          configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_ACTIVE_COLOR(true));
          g_font.printStr(16, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO START");
#else
          configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_ACTIVE_COLOR(true), 1, 2, 2, 2, 10);
          g_font.printStr(ALIGN_CENTER, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO START");
#endif
          break;
        case SIG_OP_SET_TYPE:
#if DEFAULT_TOP_OFFSET_IMAGES != 0
          configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_COLOR(true));
          g_font.printStr(28, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO SET");
#else
          configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_COLOR(true), 1, 2, 2, 2, 10);
          g_font.printStr(ALIGN_CENTER, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO SET");
#endif
          signal_drawCharts(MARGIN_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "SIG. TYPE", true);
          signal_displayValue(strSignalType, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_STRING);
          break;
        case SIG_OP_SET_MULT:
#if DEFAULT_TOP_OFFSET_IMAGES != 0
          configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_COLOR(true));
          g_font.printStr(28, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO SET");
#else
          configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_COLOR(true), 1, 2, 2, 2, 10);
          g_font.printStr(ALIGN_CENTER, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO SET");
#endif
          signal_drawCharts(MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "MULTIPLIER", true);
          signal_displayValue(strMultiplier, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_HIBRID);
          break;
        case SIG_OP_SET_FREQ:
#if DEFAULT_TOP_OFFSET_IMAGES != 0
          configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_COLOR(true));
          g_font.printStr(28, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO SET");
#else
          configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_COLOR(true), 1, 2, 2, 2, 10);
          g_font.printStr(ALIGN_CENTER, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO SET");
#endif
          signal_drawCharts(MARGIN_FILL_OUT_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(3), WIDTH_FILL_OUT_CHARTS, HEIGHT_CHARTS, "FREQUENCY", true);
          signal_displayValue(strFrequency, HORIZONTAL_PADDING_FILL_OUT_CHARTS, VERTICAL_PADDING_CHARTS(3), 0, TYPE_HIBRID);
          break;
        default:
          break;
      }
    }
    if (g_encoder.buttonPressed()) {
      if (signalOperation == SIG_OP_CANCEL) {
        alreadyDrawPanel = false;
        break;
      } else if (signalOperation == SIG_OP_COMPLETE) {
        g_encoder.mismatchEngaged();
        signal_drawCharts(MARGIN_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "SIG. TYPE", false, false);
        signal_drawCharts(MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(2), WIDTH_MIDDLE_CHARTS, HEIGHT_CHARTS, "MULTIPLIER", false, false);
        signal_drawCharts(MARGIN_FILL_OUT_CHARTS, VERTICAL_POSITION_TITLE_CHARTS(3), WIDTH_FILL_OUT_CHARTS, HEIGHT_CHARTS, "FREQUENCY", false, false);
        signal_displayValue(strSignalType, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_STRING, false);
        signal_displayValue(strMultiplier, HORIZONTAL_PADDING_MIDDLE_CHARTS + MARGIN_LEFT_CENTER_MIDDLE_CHARTS, VERTICAL_PADDING_CHARTS(2), 0, TYPE_HIBRID, false);
        signal_displayValue(strFrequency, HORIZONTAL_PADDING_FILL_OUT_CHARTS, VERTICAL_PADDING_CHARTS(3), 0, TYPE_HIBRID, false);
#if DEFAULT_TOP_OFFSET_IMAGES != 0
        g_tft.fillRect(0, HEADER_PANEL_ELEMENTS_TOP_OFFSET, LOGO_IMAGE_WIDTH, LOGO_IMAGE_HEIGHT / 2, HEADER_PANEL_COLOR);
        configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_ACTIVE_COLOR(false));
        g_font.printStr(22, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO STOP");
#else
        g_tft.fillRect(0, DEFAULT_TOP_OFFSET_IMAGES, SCR_WD, HEADER_PANEL_ELEMENTS_FRAME_HEIGHT, BACKGROUND_CHARTS_COLOR);
        g_tft.drawFastHLine(0, DEFAULT_TOP_OFFSET_IMAGES + HEADER_PANEL_ELEMENTS_FRAME_HEIGHT, SCR_WD, FRAME_PANEL_ELEMENTS_COLOR);
        g_tft.drawFastHLine(0, DEFAULT_TOP_OFFSET_IMAGES + HEADER_PANEL_ELEMENTS_FRAME_HEIGHT + 1, SCR_WD, FRAME_PANEL_ELEMENTS_COLOR);
        g_tft.drawFastHLine(0, DEFAULT_TOP_OFFSET_IMAGES + HEADER_PANEL_ELEMENTS_FRAME_HEIGHT + 2, SCR_WD, FRAME_PANEL_ELEMENTS_COLOR);
        configFont(g_font, &rreInstance_12x16, PANEL_ELEMENTS_ACTIVE_COLOR(false), 1, 2, 2, 2, 10);
        g_font.printStr(ALIGN_CENTER, HEADER_PANEL_ELEMENTS_TOP_OFFSET, "PRESS TO STOP");
#endif
        // digitalWrite(GPIO_TFT_ENABLEMENT, LOW);
        digitalWrite(GPIO_BOOSTER_ENABLEMENT, HIGH);
        delayMicroseconds(100);
        digitalWrite(GPIO_MPY634_ENABLEMENT, HIGH);
        // TODO: Implement a dinamic adjustment
        !strcmp(strSignalType, "SQUARE") ? setVoltage(analogMCPA0_agnd, (MCP4725_MAX_VALUE_RESOLUTION - 1) / 2, FALSE) : setVoltage(analogMCPA0_agnd, 288, FALSE);
        generateSignal(strFrequency, strSignalType);
        // g_FST_digipot.set(36);
        delay(1); // Try to delete this
        while (!g_encoder.buttonPressed()) {
          // TODO: Read the output signal waveform with the ADC
#if DEFAULT_TOP_OFFSET_IMAGES != 0
          if (DrewFrameWithSuccess) {
            // drawGradientMenu(false, FRAME_PANEL_ELEMENTS_HEIGHT, DEFAULT_TOP_OFFSET_IMAGES - FRAME_PANEL_ELEMENTS_HEIGHT);
            updateBatteryStatus();
            printBatteryStatus();
          }
#endif
        }
        setVoltage(analogMCPA0_agnd, 0, FALSE);
        g_signalGenerator.setWave(SIG_NONE);
        digitalWrite(GPIO_MPY634_ENABLEMENT, LOW);
        digitalWrite(GPIO_BOOSTER_ENABLEMENT, LOW);
        // digitalWrite(GPIO_TFT_ENABLEMENT, HIGH);
        spiTransaction(ILI9341_SPISettings, []() {});
        g_encoder.setPosition(SIG_OP_COMPLETE);
        signalOperation = SIG_OP_CANCEL;
      } else if (signalOperation == SIG_OP_SET_TYPE) {
        changeSettingSignal(strSignalType, signalOperation);
        g_encoder.changeRange(ENCODER_LOWER_LIMIT_SIGNAL_PANEL, ENCODER_UPPER_LIMIT_SIGNAL_PANEL);
        g_encoder.setPosition(SIG_OP_SET_TYPE);
        signalOperation = SIG_OP_CANCEL;
      } else if (signalOperation == SIG_OP_SET_MULT) {
        changeSettingSignal(strMultiplier, signalOperation);
        g_encoder.changeRange(ENCODER_LOWER_LIMIT_SIGNAL_PANEL, ENCODER_UPPER_LIMIT_SIGNAL_PANEL);
        g_encoder.setPosition(SIG_OP_SET_MULT);
        signalOperation = SIG_OP_CANCEL;
      } else if (signalOperation == SIG_OP_SET_FREQ) {
        changeSettingSignal(strFrequency, signalOperation);
        g_encoder.changeRange(ENCODER_LOWER_LIMIT_SIGNAL_PANEL, ENCODER_UPPER_LIMIT_SIGNAL_PANEL);
        g_encoder.setPosition(SIG_OP_SET_FREQ);
        signalOperation = SIG_OP_CANCEL;
      }
    }
  }
}
