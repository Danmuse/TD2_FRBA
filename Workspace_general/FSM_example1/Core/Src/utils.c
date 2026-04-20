/*
 * utils.c
 *
 *  Created on: Apr 22, 2025
 *      Author: ddimodica
 */

#include "assert_hal.h"
#include "utils.h"

DEFINE_THIS_FILE_NAME

static volatile uint32_t cpu_irq_critical_section_counter;
static volatile Bool_t   cpu_irq_prev_interrupt_state;
volatile Bool_t   g_interrupt_enabled = TRUE;

#define cpu_irq_is_enabled()    (__get_PRIMASK() == 0)

void cpu_irq_enter_critical(void)
{
	if (cpu_irq_critical_section_counter == 0)
	{
		if (cpu_irq_is_enabled())
		{
			cpu_irq_disable();
			cpu_irq_prev_interrupt_state = TRUE;
		}
		else
		{
			/* Make sure the to save the prev state as false */
			cpu_irq_prev_interrupt_state = FALSE;
		}
	}
	cpu_irq_critical_section_counter++;
}

void cpu_irq_leave_critical(void)
{
	/* Check if the user is trying to leave a critical section when not in a critical section */
	ASSERT(cpu_irq_critical_section_counter > 0);

	cpu_irq_critical_section_counter--;

	/* Only enable global interrupts when the counter reaches 0 and the state of the global interrupt flag
	   was enabled when entering critical state */
	if ((cpu_irq_critical_section_counter == 0) && (cpu_irq_prev_interrupt_state))
	{
		cpu_irq_enable();
	}
}

