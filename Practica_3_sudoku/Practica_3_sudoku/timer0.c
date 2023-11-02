/*********************************************************************************************
* Fichero:		timer.c
* Autor:
* Descrip:		TIMER CONTROL REBOTES BOTONES
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "timer0.h"
#include "44b.h"
#include "44blib.h"
#include "maquina_estados.h"

/*--- variables globales ---*/

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer0_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
void timer0_ISR(void)
{
	maquina_estado_rebotes(); //Como este timer se ocupa de los tiempos de los rebotes,
	                         //debemos llamar a la funcion que gestiona los rebotes cuando
	                         //se activa este timer
	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_TIMER0; // BIT_TIMER0 está definido en 44b.h y pone un uno en el bit 13 que correponde al Timer0
}

void timer0_init(void)
{
	/* Configuraion controlador de interrupciones */
	rINTMOD &= ~(BIT_TIMER0); // Configura las linas como de tipo IRQ
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK &= ~(BIT_TIMER0); // habilitamos en vector de mascaras de interrupcion el
	                          //Timer0 (bits 26 y 13, BIT_GLOBAL y BIT_TIMER0 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER0 */
	pISR_TIMER0 = (unsigned) timer0_ISR;

	/* Configura el Timer0 */
	rTCFG0 = 0x255; // ajusta el preescalado
	rTCFG1 = 0x0; // selecciona la entrada del mux que proporciona el reloj.
	              // La 00 corresponde a un divisor de 1/2.

	rTCMPB0 = 0x0;// valor de comparación
	rTCON =0x0; //Resetea TCON
}

//Timer para quitar rebotes al pulsar un boton
void timer0_trp() {
	rTCNTB0 = 6400; //100 ms
	//Ponemos manual update del timer 2 a 1 (bit 13)
	rTCON |=0x2;
	//Activamos auto reload y start y desactivamos manual update, a la vez que
	//respetamos los bits de timer 0 y 1
	rTCON ^= 0x3; //Ponemos este número en vez de 0x09 para no modificar los
	              //demás timers de la placa (Este timer no tiene auto-reload)
}

void timer0_boton_mantenido(){ //Debe ser autoreload para en caso
	                           //de seguir manteniendose, volver a saltar el timer
	rTCNTB0 = 3200; //50 ms
	//Ponemos manual update del timer 2 a 1 (bit 13)
	rTCON |=0x2;
	rTCON ^= 0x1;
	//Activamos auto reload y start y desactivamos manual update, a la
	//vez que respetamos los bits de timer 0 y 1
	rTCON ^= 0x3;
}

//Timer para quitar rebotes al despulsar un boton
void timer0_trd(){
	rTCNTB0 = 6400; //100 ms
	//Ponemos manual update del timer 2 a 1 (bit 13)
	rTCON |=0x2;
	rTCON ^= 0x1;
	//Activamos auto reload y start y desactivamos manual update, a
	//la vez que respetamos los bits de timer 0 y 1
	rTCON ^= 0x3;
}

//Metodo para parar el timer 0
void timer0_stop(){
	rTCON |=0x2;
	rTCON ^= 0x3; //Ponemos bit start/stop a 0

}
