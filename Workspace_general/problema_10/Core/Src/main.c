/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "interfaz_tp.h"
#include "debounce.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// #define DEBOUNCE_TICKS	(10)
// #define DEFAULT_BLINK	(500)
#define LEN_COLA_MSG (64)
#define LONG_MAX_LINEA (32)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = { .name = "defaultTask",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* USER CODE BEGIN PV */
xQueueHandle colaRx, colaTx;
char dato_rx, dato_tx;
uint8_t enviando = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */
void serial_receive_task(void *p);
void serial_send_task(void *p);
void tarea_botones(void *p);
void enviar_string_a_cola(const char *str);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
 void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
 BaseType_t HigherPriorityTaskWoken;
 static TickType_t anterior = 0;
 if(GPIO_Pin == BTN_BOARD_Pin){
 anterior = xTaskGetTickCountFromISR() - anterior;
 if(anterior > DEBOUNCE_TICKS){
 xSemaphoreGiveFromISR(semaforo,&HigherPriorityTaskWoken);
 portEND_SWITCHING_ISR(HigherPriorityTaskWoken);
 }
 }
 }
 */

void enviar_string_a_cola(const char *str) {
	while (*str) {
		char c = *str;
		xQueueSend(colaTx, (uint8_t*)&c, portMAX_DELAY);
		str++;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		BaseType_t llamarplanificador = pdFALSE;

		xQueueSendFromISR(colaRx, &dato_rx, &llamarplanificador);

		// Rearme por registros para evitar fallos de HAL_BUSY
		//SET_BIT(huart->Instance->CR1, USART_CR1_RXNEIE);
		HAL_UART_Receive_IT(huart, &dato_rx, sizeof(dato_rx));

		portEND_SWITCHING_ISR(llamarplanificador);
	}
}

/**
  * @brief Callback de transmisión de la UART por IT (Tu lógica de encadenamiento continuo)
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		BaseType_t llamarplanificador = pdFALSE;
		char dato;

		// Si quedan caracteres en la cola de transmisión, los enviamos de inmediato desde la ISR
		if (xQueueReceiveFromISR(colaTx, &dato, &llamarplanificador) == pdPASS) {
			enviando = 1;
			dato_tx = dato;
			HAL_UART_Transmit_IT(&huart2, (uint8_t*)&dato_tx, sizeof(dato_tx));
		} else {
			enviando = 0; // La cola se vació, liberamos el canal
		}
		portEND_SWITCHING_ISR(llamarplanificador);
	}
}

void serial_receive_task(void *p) {
	char buffer_linea[LONG_MAX_LINEA];
	uint8_t indice = 0;
	char dato;

	memset(buffer_linea, 0, LONG_MAX_LINEA);

	while(1) {
		// Esperamos un carácter de la ISR
		xQueueReceive(colaRx, (uint8_t*)&dato, portMAX_DELAY);

		// Omitimos el carriage return (\r) por completo
		/*
		if (dato == '\r') {
			continue;
		}
		*/

		// Si no es el fin de línea, acumulamos
		if (dato != '\r') {
			if (indice < (LONG_MAX_LINEA - 1)) {
				buffer_linea[indice++] = dato;
			}
		}
		// Llegó el '\n', procesamos el buffer acumulado
		else {
			buffer_linea[indice] = '\0'; // Cerramos el string

			// Si la línea está vacía, la ignoramos
			if (indice == 0) {
				continue;
			}

			// PARSER MANUAL: Validación estricta por posiciones fijas
			// El formato "LED X Y" tiene exactamente 7 caracteres (índices 0 al 6)
			if (indice == 7 &&
				buffer_linea[0] == 'L' &&
				buffer_linea[1] == 'E' &&
				buffer_linea[2] == 'D' &&
				buffer_linea[3] == ' ' && // Espacio obligatorio
				buffer_linea[5] == ' ')   // Espacio obligatorio
			{
				// Convertimos los caracteres numéricos ('0' o '1') a enteros (0 o 1)
				// Restar '0' es el truco clásico de ASCCI para obtener el valor numérico
				int n_led  = buffer_linea[4] - '0';
				int estado = buffer_linea[6] - '0';

				// Validamos que los valores estén en los rangos correctos
				if ((n_led == 0 || n_led == 1) && (estado == 0 || estado == 1)) {

					// Acción sobre el hardware
					setear_led(n_led, estado);

					enviar_string_a_cola("OK\r\n");
				} else {
					enviar_string_a_cola("ERROR\r\n");
				}
			}
			else {
				// No mide 7 caracteres o no empieza con "LED " o no tiene los espacios fijos
				enviar_string_a_cola("ERROR\r\n");
			}

			// Reset del buffer para la próxima trama
			indice = 0;
			memset(buffer_linea, 0, LONG_MAX_LINEA);
		}
	}
}

/*
void serial_receive_task(void *p) {
	char buffer_linea[LONG_MAX_LINEA];
	uint8_t indice = 0;
	char dato;

	memset(buffer_linea, 0, LONG_MAX_LINEA);

	while(1) {
		// Esperamos un carácter proveniente de la ISR
		xQueueReceive(colaRx, (uint8_t*)&dato, portMAX_DELAY);

		// Omitimos el carriage return (\r) si la terminal virtual lo envía
		if (dato == '\r') {
			continue;
		}

		if (dato != '\n') {
			if (indice < (LONG_MAX_LINEA - 1)) {
				buffer_linea[indice++] = dato;
			}
		}
		else {
			buffer_linea[indice] = '\0'; // Cerramos el string

			if (indice == 0) {
				continue; // Línea vacía
			}

			int n_led = -1;
			int estado = -1;

			// sscanf con el formato estricto pedido
			if (!(sscanf(buffer_linea, "LED %d %d", &n_led, &estado))) { // == 2) {
				if ((n_led == 0 || n_led == 1) && (estado == 0 || estado == 1)) {

					// Acción sobre el hardware de los LEDs
					setear_led(n_led, estado);

					enviar_string_a_cola("OK\r\n");
				} else {
					enviar_string_a_cola("ERROR\r\n");
				}
			} else {
				enviar_string_a_cola("ERROR\r\n");
			}

			indice = 0;
			memset(buffer_linea, 0, LONG_MAX_LINEA);
		}
	}
}
*/

void serial_send_task(void *p) {
	char dato;
	while(1) {
		// Si el periférico no está ocupado transmitiendo, tomamos un carácter de la cola
		if (!enviando){
			if (xQueueReceive(colaTx, (uint8_t*)&dato, portMAX_DELAY) == pdPASS) {
				enviando = 1;
				dato_tx = dato;
				HAL_UART_Transmit_IT(&huart2, (uint8_t*)&dato_tx, sizeof(dato_tx));
			}
		}
		// Pequeña pausa de control para evitar acaparar CPU si la cola está vacía
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

void tarea_botones(void *p) {
	debounce_t botones[2];
	uint16_t pines[2];

	for(int i = 0; i < 2; i++){
		debounce_inic(&botones[i]);
	}

	while(1) {
		// 1 si está presionado, 0 si está suelto (Adapta lógica inversa)
		pines[0] = (pulsador_activo(0) == 0);
		pines[1] = (pulsador_activo(1) == 0);

		for(int i = 0; i < 2; i++) {
			debounce_fsm(&botones[i], pines[i]);

			// Si detectamos flanco de presión, enviamos el mensaje carácter por carácter a colaTx
			if(botones[i].activo && !botones[i].anterior) {
				if(i == 0) {
					enviar_string_a_cola("B1\r\n");
				} else if(i == 1) {
					enviar_string_a_cola("B2\r\n");
				}
			}
			debounce_tick(&botones[i]);
		}
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Init scheduler */
	osKernelInitialize();

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	// sem_tx_completo = xSemaphoreCreateBinary();
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	// cola = xQueueCreate(4,sizeof(TickType_t));
	colaRx = xQueueCreate(LEN_COLA_MSG, sizeof(char));
	colaTx = xQueueCreate(LEN_COLA_MSG, sizeof(char));
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of defaultTask */
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL,
			&defaultTask_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	xTaskCreate(serial_receive_task, "SERIAL_RX", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(serial_send_task,    "SERIAL_TX", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(tarea_botones,       "BOTONES",   configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 2, NULL);
	/* 3. Activación inicial de la interrupción de recepción de la UART */
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&dato_rx, sizeof(dato_rx));
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 100;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, LED_BLANCO_Pin | LED_AZUL_Pin | LED_VERDE_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, LED_AMARILLO_Pin | LED_ROJO_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : BTN_BOARD_Pin */
	GPIO_InitStruct.Pin = BTN_BOARD_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(BTN_BOARD_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : BTN_3_Pin BTN_2_Pin */
	GPIO_InitStruct.Pin = BTN_3_Pin | BTN_2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : LED_BLANCO_Pin LED_AZUL_Pin LED_VERDE_Pin */
	GPIO_InitStruct.Pin = LED_BLANCO_Pin | LED_AZUL_Pin | LED_VERDE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : BTN_1_Pin */
	GPIO_InitStruct.Pin = BTN_1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BTN_1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LED_AMARILLO_Pin LED_ROJO_Pin */
	GPIO_InitStruct.Pin = LED_AMARILLO_Pin | LED_ROJO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for (;;) {
		osDelay(250);
	}
	/* USER CODE END 5 */
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
