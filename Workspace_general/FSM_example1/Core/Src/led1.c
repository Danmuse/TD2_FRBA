/*
 * led.c
 *
 *  Created on: Apr 10, 2025
 *      Author: ddimodica
 */

#include "assert_hal.h"
#include "led1.h"
#include "activeobject.h"
#include "systemsettings.h"

//DEFINE_THIS_FILE_NAME

typedef struct
{
	ActiveObject_t m_super;
	GPIO_TypeDef  *m_gpioPort;
	uint16_t       m_gpioPin;
} Led1_t;

static Led1_t l_led1;

ActiveObject_t * const AOled1 = &(l_led1.m_super);

static StateResult_t Led1_initial      ( Led1_t * const me, Event_t const e );
static StateResult_t Led1_off          ( Led1_t * const me, Event_t const e );
static StateResult_t Led1_blinking_off ( Led1_t * const me, Event_t const e );
static StateResult_t Led1_blinking_on  ( Led1_t * const me, Event_t const e );

static StateResult_t Led1_initial(Led1_t * const me, Event_t const e)
{
	return STATEMACHINE_TRANSITION(me, Led1_off);
}

static StateResult_t Led1_off(Led1_t * const me, Event_t const e)
{
	switch(e)
	{
		case SIG_ENTRY:
			HAL_GPIO_WritePin(me->m_gpioPort, me->m_gpioPin, GPIO_PIN_RESET);
			return STATEMACHINE_HANDLED();

		case SIG_ENABLE:
			return STATEMACHINE_TRANSITION(me, Led1_blinking_on);
	}
	return STATEMACHINE_IGNORED();
}

static StateResult_t Led1_blinking_off(Led1_t * const me, Event_t const e)
{
	switch(e)
	{
		case SIG_ENTRY:
			HAL_GPIO_WritePin(me->m_gpioPort, me->m_gpioPin, GPIO_PIN_RESET);
			return STATEMACHINE_HANDLED();

		case SIG_TOGGLE:
			return STATEMACHINE_TRANSITION(me, Led1_blinking_on);

		case SIG_DISABLE:
			return STATEMACHINE_TRANSITION(me, Led1_off);
	}
	return STATEMACHINE_IGNORED();
}

static StateResult_t Led1_blinking_on(Led1_t * const me, Event_t const e)
{
	switch(e)
	{
		case SIG_ENTRY:
			HAL_GPIO_WritePin(me->m_gpioPort, me->m_gpioPin, GPIO_PIN_SET);
			return STATEMACHINE_HANDLED();

		case SIG_TOGGLE:
			return STATEMACHINE_TRANSITION(me, Led1_blinking_off);

		case SIG_DISABLE:
			return STATEMACHINE_TRANSITION(me, Led1_off);
	}
	return STATEMACHINE_IGNORED();
}

void Led1_constructor(GPIO_TypeDef *gpioPort, uint16_t gpioPin)
{
	l_led1.m_gpioPort = gpioPort;
	l_led1.m_gpioPin  = gpioPin;

	ActiveObject_constructor(&(l_led1.m_super), (StateHandler_t)Led1_initial);
}
