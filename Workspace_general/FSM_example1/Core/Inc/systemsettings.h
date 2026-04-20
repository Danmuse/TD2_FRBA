/*
 * systemsettings.h
 *
 * Created: 15/12/2024 15:11:12
 *  Author: ddimodica
 */

#ifndef SYSTEMSETTINGS_H_
#define SYSTEMSETTINGS_H_

#include "activeobject.h"

enum SystemEvents
{
	SIG_SW_1_PRESSED = SIG_USER,
	SIG_SW_2_PRESSED,
	SIG_ENABLE,
	SIG_DISABLE,
	SIG_TOGGLE
};

extern ActiveObject_t * const AOapplication;
extern ActiveObject_t * const AOled1;
extern ActiveObject_t * const AOled2;

#endif
