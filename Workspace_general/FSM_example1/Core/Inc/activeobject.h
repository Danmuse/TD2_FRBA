/*
 * actor.h
 *
 * Created: 2/2/2025 11:03:35
 *  Author: ddimodica
 */ 

#ifndef ACTIVEOBJECT_H_
#define ACTIVEOBJECT_H_

#include "statemachine.h"
#include "eventqueue.h"

#define ActiveObject_constructor(me_, initial_) ( StateMachine_constructor(&((me_)->m_super), initial_) )
#define ActiveObject_dispatch(me_, e_)          ( StateMachine_dispatch(&((me_)->m_super), e_) )

typedef struct 
{
	StateMachine_t m_super;
	EventQueue_t   m_eventQueue;
} ActiveObject_t;

void    ActiveObject_start      ( ActiveObject_t * const me, Event_t *buf, uint32_t const arraySize );
void    ActiveObject_send_event ( ActiveObject_t * const me, Event_t const e );
Event_t ActiveObject_get_event  ( ActiveObject_t * const me );



#endif /* ACTOR_H_ */
