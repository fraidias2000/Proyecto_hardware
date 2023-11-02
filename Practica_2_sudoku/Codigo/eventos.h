/*
 * eventos.h
 *
 *  Created on: 23/11/2021
 *      Author: talie
 */

#ifndef EVENTOS_H_
#define EVENTOS_H_

enum ID_evento{
	TIMER_0 = 0x00,
	TIMER_1 = 0x01,
	TIMER_2 = 0x02,
	BOTON = 0x03,
	LED = 0x04,
	INICIO_SUDOKU = 0x05,
	FIN_SUDOKU = 0x06
};

enum auxData{
	//TIMER_0
	T0_START = 0x00,
	T0_STOP = 0x01,

	//TIMER_1
	T1_START = 0x02,
	T1_STOP = 0x03,

	//TIMER_2
	T2_START = 0x04,
	T2_STOP = 0x05,

	//BOTON
	IZQUIERDO = 0x06,
	DERECHO = 0x07,

	//LEDS
	LATIDO = 0x08,

	//JUEGO SUDOKU
	C_C = 0x09,
	C_ARM = 0x0A,
	ARM_C = 0x0B,
	ARM_ARM = 0x0C,
	ARM_ALL = 0x0D
};
#endif /* EVENTOS_H_ */
