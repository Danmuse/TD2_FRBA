#include "main.h"

void setear_led(int n_led, int estado)
{
	switch(n_led)
	{
		case 0: HAL_GPIO_WritePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
		case 1: HAL_GPIO_WritePin(LED_AMARILLO_GPIO_Port, LED_AMARILLO_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
		case 2: HAL_GPIO_WritePin(LED_VERDE_GPIO_Port, LED_VERDE_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
		case 3: HAL_GPIO_WritePin(LED_AZUL_GPIO_Port, LED_AZUL_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
		case 4: HAL_GPIO_WritePin(LED_BLANCO_GPIO_Port, LED_BLANCO_Pin, (estado)?GPIO_PIN_SET:GPIO_PIN_RESET); break;
	}
}

void invertir_led(int n_led)
{
	switch(n_led)
	{
		case 0: HAL_GPIO_TogglePin(LED_AMARILLO_GPIO_Port, LED_AMARILLO_Pin); break;
		case 1: HAL_GPIO_TogglePin(LED_VERDE_GPIO_Port, LED_VERDE_Pin); break;
		case 2: HAL_GPIO_TogglePin(LED_ROJO_GPIO_Port, LED_ROJO_Pin); break;
		case 3: HAL_GPIO_TogglePin(LED_AZUL_GPIO_Port, LED_AZUL_Pin); break;
		case 4: HAL_GPIO_TogglePin(LED_BLANCO_GPIO_Port, LED_BLANCO_Pin); break;
	}
}

int pulsador_activo(int n_pulsador)
{
	int ret;
	switch(n_pulsador)
	{
		case 0: ret = HAL_GPIO_ReadPin(BTN_BOARD_GPIO_Port, BTN_BOARD_Pin); break; // Debe configurarse con un Pull-Down
		case 1: ret = !(HAL_GPIO_ReadPin(BTN_1_GPIO_Port, BTN_1_Pin)); break; // Posee un Pull-Up por Hardware
		case 2: ret = !(HAL_GPIO_ReadPin(BTN_2_GPIO_Port, BTN_2_Pin)); break; // Posee un Pull-Up por Hardware
		case 3: ret = !(HAL_GPIO_ReadPin(BTN_3_GPIO_Port, BTN_3_Pin)); break; // Posee un Pull-Up por Hardware
	}
	return !ret;
}

