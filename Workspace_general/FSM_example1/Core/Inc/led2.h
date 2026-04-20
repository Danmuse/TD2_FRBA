/*
 * led.h
 *
 *  Created on: Apr 10, 2025
 *      Author: ddimodica
 */

#ifndef INC_LED2_H_
#define INC_LED2_H_

#include "stm32f4xx_hal.h"

void Led2_constructor ( GPIO_TypeDef *gpioPort, uint16_t gpioPin );

#endif /* INC_LED1_H_ */
