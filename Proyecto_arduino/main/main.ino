#include "main.h"

operation_t instr_operation = OP_NONE;

void setup(void) {
  Serial.begin(115200);
  pinMode(GPIO_TFT_ENABLEMENT,     OUTPUT);
  pinMode(GPIO_BOOSTER_ENABLEMENT, OUTPUT);
  pinMode(GPIO_MPY634_ENABLEMENT,  OUTPUT);
  pinMode(GPIO_OPA564_ENABLEMENT,  OUTPUT);
  pinMode(GPIO_FSTCAP_ENABLEMENT,  OUTPUT);
  pinMode(GPIO_SNDCAP_ENABLEMENT,  OUTPUT);
  pinMode(GPIO_TRDCAP_ENABLEMENT,  OUTPUT);
  pinMode(GPIO_FTHCAP_ENABLEMENT,  OUTPUT);
  // pinMode(GPIO_BAT_CHARGING,       INPUT);
  // pinMode(GPIO_BAT_FULL,           INPUT);
  // pinMode(GPIO_BAT_SEMI_FULL,      INPUT);
  // pinMode(GPIO_BAT_SEMI_EMPTY,     INPUT);

  delay(750);
  initMCP();
  setVoltage(analogMCPA0_agnd, 0, FALSE); // TODO: Write the initial value into EEPROM (maybe, 288 into the second parameter)
  g_signalGenerator.begin();
  if (!(g_flash.begin())) flashPlugged = true;

  g_tft.begin();
  g_tft.setRotation(2);
  g_tft.clearScreen();
  g_font.init(customRect, SCR_WD, SCR_HT);

  digitalWrite(GPIO_BOOSTER_ENABLEMENT, LOW);

  digitalWrite(GPIO_TFT_ENABLEMENT, HIGH);
  if (flashPlugged) printFromFlash(INSTR_IMG);
  else drawGradientFault(ERR_FLASH_NOT_FOUND);
  g_encoder.changeRange(ENCODER_LOWER_LIMIT_MENU, ENCODER_UPPER_LIMIT_MENU);
  delay(2000);
  g_tft.fillRect(0, 0, SCR_WD, SCR_HT, DEFAULT_BACKGROUND_COLOR); // Bug fixed
  g_tft.clearScreen(); // Bug fixed
  /*
  // TESTING POWER CONSUMPTIONS:
  g_tft.fillRect(0, 0, SCR_WD, SCR_HT, 0xFFFF); // Bug fixed
  digitalWrite(GPIO_TFT_ENABLEMENT, LOW);
  delay(10000);
  
  digitalWrite(GPIO_TFT_ENABLEMENT, HIGH);
  delay(10000);

  digitalWrite(GPIO_MPY634_ENABLEMENT, HIGH);
  delay(10000);
  
  digitalWrite(GPIO_OPA564_ENABLEMENT, HIGH);
  delay(10000);

  digitalWrite(GPIO_TFT_ENABLEMENT,     LOW);
  digitalWrite(GPIO_MPY634_ENABLEMENT,  LOW);
  digitalWrite(GPIO_OPA564_ENABLEMENT,  LOW);
  digitalWrite(GPIO_BOOSTER_ENABLEMENT, HIGH);
  delay(10000);

  digitalWrite(GPIO_TFT_ENABLEMENT, HIGH);
  delay(10000);

  digitalWrite(GPIO_MPY634_ENABLEMENT, HIGH);
  delay(10000);
  
  digitalWrite(GPIO_OPA564_ENABLEMENT, HIGH);
  delay(10000);
  
  digitalWrite(GPIO_TFT_ENABLEMENT,     LOW);
  digitalWrite(GPIO_MPY634_ENABLEMENT,  LOW);
  digitalWrite(GPIO_OPA564_ENABLEMENT,  LOW);
  digitalWrite(GPIO_BOOSTER_ENABLEMENT, LOW);
  exit(0);
  */
}

void loop(void) {
  displayMenu(instr_operation);
  processOperation(instr_operation);
  // exit(0);
}
