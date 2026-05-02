#ifndef _PROCESS_ACTIONS_H_
#define _PROCESS_ACTIONS_H_

#include "MCP4725Ax.h"
#include "AD9833_DDS.h"

#define FLOAT_DECIMALS_PRECISION 4
#define SCIENTIFIC_NOTATION_EXPONENTIAL_POSITION 4

void generateSignal(char *strFrequency, char *strSignalType);

#endif // _PROCESS_ACTIONS_H_
