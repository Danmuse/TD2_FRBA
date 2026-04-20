/*
 * eventqueue.h
 *
 * Created: 31/1/2025 16:54:21
 *  Author: ddimodica
 */ 


#ifndef EVENTQUEUE_H_
#define EVENTQUEUE_H_

#include "event.h"
#include "types.h"

#define EVENT_QUEUE_MODE_FIFO            0
#define EVENT_QUEUE_MODE_CIRCULAR_BUFFER 1
#define EVENT_QUEUE_MODE EVENT_QUEUE_MODE_FIFO

// Queue de elementos del tipo Event_t. En esta implementaci?n, los eventos son de tama�o fijo.
// En un futuro, podr�an ser de tama�o variable y para eso, esta queue ser�a modificada para ser
// Una Queue de punteros a elementos del tipo Event_t.
typedef struct
{
	Event_t *m_buffer;
	Event_t *m_front;
	Event_t *m_back;
	uint32_t m_size;
	uint32_t m_arraySize;
} EventQueue_t;

void    EventQueue_constructor ( EventQueue_t * const me, Event_t *buf, uint32_t const arraySize );
void    EventQueue_enqueue     ( EventQueue_t * const me, Event_t const e );
Bool_t  EventQueue_dequeue     ( EventQueue_t * const me, Event_t * const e );
Bool_t  EventQueue_is_empty    ( EventQueue_t * const me );
Bool_t  EventQueue_is_full     ( EventQueue_t * const me );

#endif /* EVENTQUEUE_H_ */