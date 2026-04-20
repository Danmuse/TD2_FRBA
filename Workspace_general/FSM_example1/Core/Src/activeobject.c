/*
 * actor.c
 *
 * Created: 2/2/2025 11:04:18
 *  Author: ddimodica
 */ 

#include "assert_hal.h"
#include "activeobject.h"
#include "utils.h"

DEFINE_THIS_FILE_NAME

void ActiveObject_start(ActiveObject_t * const me, Event_t *buf, uint32_t const arraySize)
{
	ASSERT(me!=0);

	EventQueue_constructor(&me->m_eventQueue, buf, arraySize);
	StateMachine_init(&me->m_super);
}

void ActiveObject_send_event(ActiveObject_t * const dest, Event_t const e)
{
	
	ASSERT(dest!=0);
	
	cpu_irq_enter_critical();
	EventQueue_enqueue(&dest->m_eventQueue, e);
	cpu_irq_leave_critical();
}

Event_t ActiveObject_get_event(ActiveObject_t * const me)
{
	Event_t aux;

	ASSERT(me!=0);

	cpu_irq_enter_critical();
	if( EventQueue_dequeue(&me->m_eventQueue, &aux) == FALSE ) 
	{
		aux = SIG_NONE;
	}
	cpu_irq_leave_critical();
	
	return aux;
}
