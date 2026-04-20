/*
 * utils.h
 *
 *  Created on: Apr 22, 2025
 *      Author: ddimodica
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "stm32f4xx_hal.h"
#include "types.h"

extern volatile Bool_t g_interrupt_enabled;

#  define cpu_irq_enable()                     \
	do {                                       \
		g_interrupt_enabled = TRUE;            \
		__DMB();                               \
		__enable_irq();                        \
	} while (0)
#  define cpu_irq_disable()                    \
	do {                                       \
		__disable_irq();                       \
		__DMB();                               \
		g_interrupt_enabled = FALSE;           \
	} while (0)

void cpu_irq_enter_critical(void);
void cpu_irq_leave_critical(void);


#endif /* INC_UTILS_H_ */
