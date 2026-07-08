/*
 * debounce.c
 *
 *  Created on: Jul 1, 2026
 *      Author: CNEA
 */

#ifndef SRC_DEBOUNCE_C_
#define SRC_DEBOUNCE_C_
#include "debounce.h"

void debounce_inic(debounce_t *estado) {
	estado->activo   = 0;
	estado->anterior = 0;
	estado->fsm      = 0;
	estado->tiempo   = 0;
}

void debounce_tick(debounce_t *estado){
	if(estado->tiempo){
		(estado->tiempo)--;
	}
}

void debounce_fsm(debounce_t *estado, uint16_t pin_activo){
	uint16_t fsm = estado->fsm;
	uint16_t anterior = estado->activo;
	switch(fsm){
	case 0:
		if(pin_activo){
			fsm++;
			estado->tiempo = 20;
		}
		break;
	case 1:
		if(estado->tiempo == 0){
			if(pin_activo) {
				fsm++;
				estado->activo = 1;
			}
			else {
				fsm--;
			}
		}
		break;
	case 2:
		if(!pin_activo){
			fsm++;
			estado->tiempo = 20;
		}
		break;
	case 3:
		if(estado->tiempo == 0) {
			if(!pin_activo) {
				fsm = 0;
				estado->activo = 0;
			}
			else {
				fsm--;
			}
		}
		break;
	}
	estado->anterior = anterior;
	estado->fsm = fsm;
}



#endif /* SRC_DEBOUNCE_C_ */
