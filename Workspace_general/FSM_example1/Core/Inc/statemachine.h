/*
 * stateMachine.h
 *
 * Created: 15/12/2024 11:59:45
 *  Author: ddimodica
 */ 

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include "event.h"

enum ReservedSignals
{
	SIG_NONE  = 0,
	SIG_ENTRY = 1,
	SIG_EXIT  = 2,
	SIG_INIT  = 3,
	SIG_USER  = 4
};

typedef enum
{
	RES_HANDLED    = 0,
	RES_IGNORED    = 1,
	RES_TRANSITION = 2
} StateResult_t;

#define STATEMACHINE_HANDLED() (RES_HANDLED)
#define STATEMACHINE_IGNORED() (RES_IGNORED)
#define STATEMACHINE_TRANSITION(me_,target_) (((StateMachine_t*)me_)->m_state = (StateHandler_t)(target_), RES_TRANSITION)

typedef StateResult_t (*StateHandler_t) (void * const me, Event_t const e);

typedef struct
{
	StateHandler_t m_state;
} StateMachine_t;

void    StateMachine_constructor ( StateMachine_t * const me, StateHandler_t initial );
void    StateMachine_init        ( StateMachine_t * const me );
void    StateMachine_dispatch    ( StateMachine_t * const me, Event_t const e );

#endif /* STATEMACHINE_H_ */