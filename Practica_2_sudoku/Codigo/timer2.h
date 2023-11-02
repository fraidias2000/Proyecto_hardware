/*********************************************************************************************
* Fichero:		timer.h
* Autor:
* Descrip:		funciones de control del timer0 del s3c44b0x
* Version:
*********************************************************************************************/

#ifndef _TIMER2_H_
#define _TIMER2_H_

/*--- variables visibles del módulo timer.c/timer.h ---*/
int timer2_num_int;
int VALOR_INICIAL_TIMER2;
int variable_latido;

/*--- declaracion de funciones visibles del módulo timer.c/timer.h ---*/
void timer2_init(void);

#endif /* _TIMER2_H_ */
