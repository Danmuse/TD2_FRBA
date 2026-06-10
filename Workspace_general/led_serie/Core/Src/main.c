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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cola.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define LEN_USART_BUFFERS 	(256)
#define ESPERA_LED_MS 		(250)
#define MAX_CMD_LEN			(16)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t rxserie_buffer[LEN_USART_BUFFERS];
uint8_t txserie_buffer[LEN_USART_BUFFERS];
uint8_t dato_rx;
uint8_t dato_tx;
uint8_t tx_en_progreso;
cola_t cola_rx;
cola_t cola_tx;
int congela_led;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2){
	        if(cola_haydatos(&cola_tx)){
	            dato_tx = cola_desencolar(&cola_tx);
	            tx_en_progreso = 1;
	            HAL_UART_Transmit_IT(huart, &dato_tx, 1);
	        }
	        else {
	        	tx_en_progreso = 0;
	        }
	    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2){
		cola_encolar(&cola_rx, dato_rx);
		HAL_UART_Receive_IT(huart, &dato_rx, sizeof(dato_rx));
	}
}

void encolar_string(cola_t *cola, char*s){
	while(*s){
		cola_encolar(cola, *s++);
	}
}

void procesar_comando(char *cmd)
{
    cmd[strcspn(cmd, "\r\n")] = '\0';
    for(int i = 0; cmd[i] != '\0'; i++){
        cmd[i] = tolower((unsigned char)cmd[i]);
    }
    if(!strcmp(cmd, "led on")){
        congela_led = 1;
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
        __disable_irq();
        encolar_string(&cola_tx, "OK. LED ON\r\n");
        __enable_irq();
    }
    else if(!strcmp(cmd, "led off")){
        congela_led = 1;
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        __disable_irq();
        encolar_string(&cola_tx, "OK. LED OFF\r\n");
        __enable_irq();
    }
    else if(!strcmp(cmd, "led toggle")){
        congela_led = 1;
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        __disable_irq();
        encolar_string(&cola_tx, "OK. LED TOGGLE\r\n");
        __enable_irq();
    }
    else if(!strcmp(cmd, "led blink")){
        congela_led = 0;
        __disable_irq();
        encolar_string(&cola_tx, "OK. LED BLINKING\r\n");
        __enable_irq();
    }
    else if(!strcmp(cmd, "led status")){
        __disable_irq();
        if(HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin) == GPIO_PIN_SET){
            encolar_string(&cola_tx, "OK. LED [ON]\r\n");
        }
        else {
            encolar_string(&cola_tx, "OK. LED [OFF]\r\n");
        }
        __enable_irq();
    }
    else {
        __disable_irq();
        encolar_string(&cola_tx,
                       "ERROR. <LED ON|OFF|TOGGLE|BLINK|STATUS>\r\n");
        __enable_irq();
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  int  icmd = 0;
  int  haycomando=0;
  char cmd[LEN_USART_BUFFERS];
  uint32_t demora_led;
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
  cola_inicializar(&cola_rx, rxserie_buffer, LEN_USART_BUFFERS);
  cola_inicializar(&cola_tx, txserie_buffer, LEN_USART_BUFFERS);
  tx_en_progreso = 0;
  congela_led = 0;
  HAL_UART_Receive_IT(&huart2, &dato_rx, sizeof(dato_rx));
  demora_led = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	if(cola_haydatos(&cola_rx)){
		char v = cola_desencolar(&cola_rx);
		if (icmd < (MAX_CMD_LEN - 1)){
			cmd[icmd++] = v;
		}
		else {
			icmd = 0;
		}
		if(v == '\n'){
			cmd[icmd] = '\0';
		    haycomando = 1;
		    icmd = 0;
		}
	}
	if(haycomando){
		haycomando = 0;
		procesar_comando(cmd);
	}

	__disable_irq();
	if(cola_haydatos(&cola_tx) && (!tx_en_progreso)) {
		dato_tx = cola_desencolar(&cola_tx);
		tx_en_progreso = 1;
		__enable_irq();
		HAL_UART_Transmit_IT(&huart2, &dato_tx, 1);
	}
	else {
		__enable_irq();
	}

	if((HAL_GetTick()-demora_led)>=ESPERA_LED_MS){
		demora_led = HAL_GetTick();
		if(!congela_led){
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		}
	}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
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
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
