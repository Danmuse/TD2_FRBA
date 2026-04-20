/*
 * eventqueue.c
 *
 * Created: 31/1/2025 16:53:55
 *  Author: ddimodica
 */ 

#include "assert_hal.h"
#include "stm32f4xx_hal.h"
#include "eventqueue.h"

DEFINE_THIS_FILE_NAME

#define FORWARD_POINTER(p_) ( p_ == &(me->m_buffer[me->m_arraySize-1]) ? (p_ = me->m_buffer) : (p_++) )

void EventQueue_constructor(EventQueue_t * const me, Event_t *buf, uint32_t const arraySize) 
{
	ASSERT(me!=NULL);
	ASSERT(buf!=NULL);
	ASSERT(arraySize > 0);
	
	me->m_buffer = buf;
	me->m_front  = buf;
	me->m_back = buf;
	me->m_arraySize = arraySize;
	me->m_size = 0;
}

void EventQueue_enqueue(EventQueue_t * const me, Event_t const e) 
{
	ASSERT(me!=NULL);
	
#if EVENT_QUEUE_MODE == EVENT_QUEUE_MODE_FIFO
	ASSERT(EventQueue_is_full(me) == FALSE);
	
#elif EVENT_QUEUE_MODE == EVENT_QUEUE_MODE_CIRCULAR_BUFFER
	if(EventQueue_is_full(me))
	{
		FORWARD_POINTER(me->m_front);
		me->m_size--;
	}
#else
	#error "Invalid mode EventQueue."
#endif

	*(me->m_back) = e;
	FORWARD_POINTER(me->m_back);
	me->m_size++;	
}

// Si est� vac�a devuelve FALSE
// Si no est� vac�a devuelve TRUE
Bool_t EventQueue_dequeue(EventQueue_t * const me, Event_t * const e) 
{
	Bool_t ret = TRUE;
	
	ASSERT(me!=NULL);
	ASSERT(e!=NULL);
	
	if(!EventQueue_is_empty(me)) 
	{
		(*e) = *(me->m_front);
		FORWARD_POINTER(me->m_front);
		me->m_size--;
	}
	else 
	{
		ret = FALSE;
	}
	
	return ret;
}

Bool_t EventQueue_is_empty(EventQueue_t * const me) 
{
	ASSERT(me!=NULL);
	return (me->m_size == 0) ? TRUE : FALSE;
}

Bool_t EventQueue_is_full(EventQueue_t * const me)
{
	ASSERT(me!=NULL);
	return (me->m_size == me->m_arraySize) ? TRUE : FALSE;
}
