/*
 * debounce.h
 *
 *  Created on: Jul 1, 2026
 *      Author: CNEA
 */

#ifndef INC_DEBOUNCE_H_
#define INC_DEBOUNCE_H_
#include <stdint.h>

typedef struct {
		uint16_t tiempo;
		uint16_t fsm;
		uint16_t activo;
		uint16_t anterior;
	}debounce_t;


	void debounce_inic(debounce_t *estado);
	void debounce_tick(debounce_t *estado);
	void debounce_fsm(debounce_t *estado, uint16_t pin_activo);

#endif /* INC_DEBOUNCE_H_ */
