/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW_ENC_Pin GPIO_PIN_13
#define SW_ENC_GPIO_Port GPIOC
#define DT_ENC_Pin GPIO_PIN_14
#define DT_ENC_GPIO_Port GPIOC
#define CLK_ENC_Pin GPIO_PIN_15
#define CLK_ENC_GPIO_Port GPIOC
#define EN_FSTCAP_Pin GPIO_PIN_0
#define EN_FSTCAP_GPIO_Port GPIOC
#define EN_SNDCAP_Pin GPIO_PIN_1
#define EN_SNDCAP_GPIO_Port GPIOC
#define EN_TRDCAP_Pin GPIO_PIN_2
#define EN_TRDCAP_GPIO_Port GPIOC
#define EN_FTHCAP_Pin GPIO_PIN_3
#define EN_FTHCAP_GPIO_Port GPIOC
#define LED_STATUS_Pin GPIO_PIN_1
#define LED_STATUS_GPIO_Port GPIOA
#define DAC_DDS_CLAMP_Pin GPIO_PIN_4
#define DAC_DDS_CLAMP_GPIO_Port GPIOA
#define DAC_VCLAMP_Pin GPIO_PIN_5
#define DAC_VCLAMP_GPIO_Port GPIOA
#define CH_BT_Pin GPIO_PIN_7
#define CH_BT_GPIO_Port GPIOA
#define EN_OPAMP_Pin GPIO_PIN_1
#define EN_OPAMP_GPIO_Port GPIOB
#define DSCK_Pin GPIO_PIN_2
#define DSCK_GPIO_Port GPIOB
#define EN_AMP_Pin GPIO_PIN_10
#define EN_AMP_GPIO_Port GPIOB
#define RS_TFT_Pin GPIO_PIN_6
#define RS_TFT_GPIO_Port GPIOC
#define DC_TFT_Pin GPIO_PIN_7
#define DC_TFT_GPIO_Port GPIOC
#define CS_TOUCH_Pin GPIO_PIN_8
#define CS_TOUCH_GPIO_Port GPIOC
#define DSPI_IO0_Pin GPIO_PIN_9
#define DSPI_IO0_GPIO_Port GPIOC
#define LED_TFT_AUX_Pin GPIO_PIN_8
#define LED_TFT_AUX_GPIO_Port GPIOA
#define TMP_ALERT_Pin GPIO_PIN_9
#define TMP_ALERT_GPIO_Port GPIOA
#define CFG_DCDC_Pin GPIO_PIN_10
#define CFG_DCDC_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define DSPI_IO1_Pin GPIO_PIN_10
#define DSPI_IO1_GPIO_Port GPIOC
#define BT_25_Pin GPIO_PIN_11
#define BT_25_GPIO_Port GPIOC
#define BT_50_Pin GPIO_PIN_12
#define BT_50_GPIO_Port GPIOC
#define BT_75_Pin GPIO_PIN_2
#define BT_75_GPIO_Port GPIOD
#define BT_100_Pin GPIO_PIN_4
#define BT_100_GPIO_Port GPIOB
#define CS_W25Q_Pin GPIO_PIN_6
#define CS_W25Q_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
