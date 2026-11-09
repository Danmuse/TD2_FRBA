/*
 * Temp.cc
 *
 *  Created on: Nov 7, 2026
 *      Author: SVL
 */

#include "Temp.h"

static I2C_HandleTypeDef *temp_i2c;

static volatile uint8_t temp_alert_flag = 0;

void tempEXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == TMP_ALERT_Pin)
    {
        temp_alert_flag = 1;
    }
}

uint8_t tempAlertDetected(void)
{
    if (temp_alert_flag)
    {
        temp_alert_flag = 0;
        return 1;
    }

    return 0;
}

static HAL_StatusTypeDef tempWRITEREGISTRO(uint8_t reg, uint16_t value)
{
    uint8_t data[3];

    data[0] = reg;
    data[1] = (uint8_t)(value >> 8);
    data[2] = (uint8_t)(value & 0xFF);

    return HAL_I2C_Master_Transmit(temp_i2c, TEMP_I2C_ADDRESS, data, 3, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef tempREADREGISTRO(uint8_t reg, uint16_t *value)
{
    uint8_t data[2];

    if (HAL_I2C_Master_Transmit(temp_i2c, TEMP_I2C_ADDRESS, &reg, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (HAL_I2C_Master_Receive(temp_i2c, TEMP_I2C_ADDRESS, data, 2, HAL_MAX_DELAY) != HAL_OK)
    {
        return HAL_ERROR;
    }

    *value = ((uint16_t)data[0] << 8) | data[1];

    return HAL_OK;
}

//INIT que habilita la alerta en modo interrupción, enviar los valores de temperatura en grados
HAL_StatusTypeDef tempINIT(I2C_HandleTypeDef *hi2c, float Tupper, float Tcrit, float Tlower)
{
    temp_i2c = hi2c;

    /* Configurar TUPPER */
    if (tempWRITEREGISTRO(TEMP_REG_TUPPER, (uint16_t)(Tupper * 16.0f)) != HAL_OK)
        return HAL_ERROR;

    /* Configurar TLOWER */
    if (tempWRITEREGISTRO(TEMP_REG_TLOWER, (uint16_t)(Tlower * 16.0f)) != HAL_OK)
        return HAL_ERROR;

    /* Configurar TCRIT */
    if (tempWRITEREGISTRO(TEMP_REG_TCRIT, (uint16_t)(Tcrit  * 16.0f)) != HAL_OK)
        return HAL_ERROR;

    /*
     * CONFIG = 0x0029
     *
     * Bit 3 = 1: ALERT habilitado
     * Bit 2 = 0: Comparar TUPPER, TLOWER y TCRIT
     * Bit 1 = 0: ALERT activa en LOW
     * Bit 0 = 1: Interrupt Mode
     */

    if (tempWRITEREGISTRO(TEMP_REG_CONFIG, 0x0029) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

//Funcion para leer la temperatura y guardarla en el parámetro
HAL_StatusTypeDef tempREAD(float *temperature)
{
    uint16_t raw;

    if (tempREADREGISTRO(TEMP_REG_AMBIENT, &raw) != HAL_OK)
        return HAL_ERROR;

    uint16_t tempData = raw & 0x0FFF;

    *temperature = ((tempData >> 4) * 1.0f) + ((tempData & 0x000F) * 0.0625f);

    return HAL_OK;
}


//Llamar a esta función despues de atender a la interrupción para que se rehabilite
HAL_StatusTypeDef tempCLEARINT(void)
{
    return tempWRITEREGISTRO(TEMP_REG_CONFIG, 0x0029 | 0x0020);
}
