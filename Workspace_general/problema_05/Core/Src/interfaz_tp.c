#include "main.h"

void setear_led(int n_led, int estado)
{
	switch(n_led)
	{
		case 0: HAL_GPIO_WritePin(LED_AMARILLO_GPIO_Port, LED_AMARILLO_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
		case 1: HAL_GPIO_WritePin(LED_VERDE_GPIO_Port, LED_VERDE_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
		case 2: HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
	}
}

void invertir_led(int n_led)
{
	switch(n_led)
	{
		case 0: HAL_GPIO_TogglePin(LED_AMARILLO_GPIO_Port, LED_AMARILLO_Pin); break;
		case 1: HAL_GPIO_TogglePin(LED_VERDE_GPIO_Port, LED_VERDE_Pin); break;
		case 2: HAL_GPIO_TogglePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin); break;
	}
}

int pulsador_activo(int n_pulsador)
{
	int ret;
	switch(n_pulsador)
	{
		case 0: ret = HAL_GPIO_ReadPin(BTN_BOARD_GPIO_Port, BTN_BOARD_Pin); break;
		case 1: ret = HAL_GPIO_ReadPin(BTN_1_GPIO_Port, BTN_1_Pin); break;
	}
	return !ret;
}
