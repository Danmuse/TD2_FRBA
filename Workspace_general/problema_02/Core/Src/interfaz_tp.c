#include "main.h"

void setear_led(int n_led, int estado)
{
	switch(n_led)
	{
		case 0: HAL_GPIO_WritePin(LED_AMARILLO_GPIO_Port, LED_AMARILLO_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
		case 1: HAL_GPIO_WritePin(LED_VERDER_GPIO_Port, LED_VERDER_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
		case 2: HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
	}
}

void invertir_led(int n_led)
{
	switch(n_led)
	{
		case 0: HAL_GPIO_TogglePin(LED_AMARILLO_GPIO_Port, LED_AMARILLO_Pin); break;
		case 1: HAL_GPIO_TogglePin(LED_VERDER_GPIO_Port, LED_VERDER_Pin); break;
		case 2: HAL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin); break;
	}
}

int pulsador_activo(int n_pulsador)
{
	int ret;
	switch(n_pulsador)
	{
		case 0: ret = HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin); break;
		case 1: ret = HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin); break;
	}
	return !ret;
}

