/*
 * assert_HTC.c
 *
 * Created: 29/1/2025 10:11:05
 *  Author: ddimodica
 */ 

#include "assert_hal.h"
#include "stm32f4xx_hal.h"
#include "utils.h"

void onAssert(char const * file, int line)
{
	(void)file; // <-- esto es para que el compilador no llore
	(void)line; // <-- esto es para que el compilador no llore
	
	// Deshabilitar todas las interrupciones
	cpu_irq_disable();

	// Prender led de error [se comenta porque, en caso de que se clave el sensor ac�, luego
	// va a saltar el watchdog y este s� encender� el LED de error y lo indicar� en el input
	// register del teSensor]
	//LEDerror_turn_on();
	
	// El c�dgio se bloquea para siempre, esperando a que salte el watchdog y reinicie la placa.
	while (1);
}
