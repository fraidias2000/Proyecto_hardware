/*********************************************************************************************
* Fichero:		timer0.h
* Autor:
* Descrip:		TIMER CONTROL REBOTES BOTONES
* Version:
*********************************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

/*--- variables visibles del módulo timer.c/timer.h ---*/
/*--- declaracion de funciones visibles del módulo timer.c/timer.h ---*/
void timer0_init(void);
void timer0_trp();
void timer0_boton_mantenido();
void timer0_trd();
void timer0_stop();

#endif /* _TIMER_H_ */
