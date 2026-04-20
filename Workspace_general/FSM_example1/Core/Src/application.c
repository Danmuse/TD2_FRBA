/*
 * application.c
 *
 *  Created on: Mar 28, 2025
 *      Author: ddimodica
 */
#include "assert_hal.h"
#include "stm32f4xx_hal.h"
#include "application.h"
#include "activeobject.h"
#include "systemsettings.h"

DEFINE_THIS_FILE_NAME

typedef struct
{
	ActiveObject_t     m_super;
	TIM_HandleTypeDef *m_htim;
} Application_t;

static Application_t l_application;

ActiveObject_t * const AOapplication = &(l_application.m_super);

static StateResult_t Application_initial ( Application_t * const me, Event_t const e );
static StateResult_t Application_idle    ( Application_t * const me, Event_t const e );
static StateResult_t Application_active  ( Application_t * const me, Event_t const e );

static StateResult_t Application_initial(Application_t * const me, Event_t const e)
{
	return STATEMACHINE_TRANSITION(me, Application_idle);
}

static StateResult_t Application_idle(Application_t * const me, Event_t const e)
{
	switch(e)
	{
		case SIG_SW_1_PRESSED:
			return STATEMACHINE_TRANSITION(me, Application_active);
	}
	return STATEMACHINE_IGNORED();
}

static StateResult_t Application_active(Application_t * const me, Event_t const e)
{
	switch(e)
	{
		case SIG_ENTRY:
			ActiveObject_send_event(AOled1, SIG_ENABLE);
			ActiveObject_send_event(AOled2, SIG_ENABLE);
			HAL_TIM_Base_Start_IT(me->m_htim);
			return STATEMACHINE_HANDLED();

		case SIG_SW_2_PRESSED:
			//ActiveObject_send_event(AOled1, SIG_TOGGLE);
			ActiveObject_send_event(AOled2, SIG_TOGGLE);
			return STATEMACHINE_HANDLED();

		case SIG_SW_1_PRESSED:
			return STATEMACHINE_TRANSITION(me, Application_idle);

		case SIG_EXIT:
			HAL_TIM_Base_Stop(me->m_htim);
			ActiveObject_send_event(AOled1, SIG_DISABLE);
			ActiveObject_send_event(AOled2, SIG_DISABLE);
			return STATEMACHINE_HANDLED();
	}
	return STATEMACHINE_IGNORED();
}

void Application_constructor(TIM_HandleTypeDef * const htim)
{
	ASSERT(htim!=0);

	l_application.m_htim = htim;

	ActiveObject_constructor(&(l_application.m_super), (StateHandler_t)Application_initial);
}
