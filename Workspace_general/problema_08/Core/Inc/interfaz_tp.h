/*
 * interfaz_tp.h
 *
 *  Created on: Jun 23, 2026
 *      Author: usuario
 */

#ifndef INC_INTERFAZ_TP_H_
#define INC_INTERFAZ_TP_H_
#define MAX_LEDS	(4)

void setear_led(int n_led, int estado);
void invertir_led(int n_led);
int pulsador_activo(int n_pulsador);

#endif /* INC_INTERFAZ_TP_H_ */
