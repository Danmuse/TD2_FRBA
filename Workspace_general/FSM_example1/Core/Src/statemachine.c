/*
 * statemachine.c
 *
 * Created: 15/12/2024 12:41:41
 *  Author: ddimodica
 */ 

#include "stateMachine.h"

void StateMachine_constructor(StateMachine_t * const me, StateHandler_t initial)
{
	me->m_state = initial;
}

void StateMachine_init(StateMachine_t * const me)
{
	(me->m_state)(me, SIG_NONE);
	(me->m_state)(me, SIG_ENTRY);
}

void StateMachine_dispatch(StateMachine_t * const me, Event_t const e)
{
	StateHandler_t s = me->m_state;
	StateResult_t r = (*s)(me,e);
	
	if(r == RES_TRANSITION)
	{
		(*s)(me, SIG_EXIT);
		(me->m_state)(me, SIG_ENTRY);
	}
}
