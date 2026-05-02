#include "processActions.h"

void generateSignal(char *strFrequency, char *strSignalType) {
  spiTransaction(AD9833_SPISettings, []() { });
  if (strFrequency[strlen(strFrequency) - 2] == 'H' && strFrequency[strlen(strFrequency) - 1] == 'z') {
    for (byte idx = 0; idx < strlen(strFrequency) && strFrequency[idx] != 'H'; idx++)
      if (!isNumberExt(strFrequency[idx])) return;
    char strFrequencyValue[strlen(strFrequency) - 2 + 1];
    strncpy(strFrequencyValue, strFrequency, strlen(strFrequency) - 2);
    strFrequencyValue[strlen(strFrequency) - 2] = '\0';
    if (!strcmp(strSignalType, "SINE")) g_signalGenerator.configDDS(atoi(strFrequencyValue), SIG_SINE);
    else if (!strcmp(strSignalType, "SQUARE")) g_signalGenerator.configDDS(atoi(strFrequencyValue), SIG_FST_SQUARE);
    else if (!strcmp(strSignalType, "TRIANG")) g_signalGenerator.configDDS(atoi(strFrequencyValue), SIG_TRIANGLE);
  }
}
