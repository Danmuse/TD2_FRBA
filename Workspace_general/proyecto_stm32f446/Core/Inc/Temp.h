/*
 * Temp.h
 *
 *  Created on: Nov 7, 2026
 *      Author: SVL
*/

#ifndef TEMP_H
#define TEMP_H

#include "main.h"
#include <stdint.h>

#define TEMP_I2C_ADDRESS    (0x18 << 1)

#define TEMP_REG_CONFIG     0x01
#define TEMP_REG_TUPPER     0x02
#define TEMP_REG_TLOWER     0x03
#define TEMP_REG_TCRIT      0x04
#define TEMP_REG_AMBIENT    0x05

#define TEMP_UPPER_LIMIT    30.0f

HAL_StatusTypeDef tempINIT(I2C_HandleTypeDef *hi2c, float Tupper, float Tcrit, float Tlower);
HAL_StatusTypeDef tempREAD(float *temperature);
HAL_StatusTypeDef tempCLEARINT(void);

/* Gestion de interrupciones */
void tempEXTI_Callback(uint16_t GPIO_Pin);
uint8_t tempAlertDetected(void);

#endif
