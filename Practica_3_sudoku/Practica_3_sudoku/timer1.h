/*********************************************************************************************
* Fichero:		timer.h
* Autor:
* Descrip:		funciones de control del timer0 del s3c44b0x
* Version:
*********************************************************************************************/

#ifndef _TIMER1_H_
#define _TIMER1_H_

/*--- variables globales ---*/
int timer1_num_int;
int VALOR_INICIAL_TIMER1;

/*--- declaracion de funciones visibles del módulo timer1.c/timer1.h ---*/
void timer1_ISR(void);
void timer1_init(void);
void timer1_start();
unsigned int timer1_count();
unsigned int timer1_stop();
#endif /* _TIMER1_H_ */
