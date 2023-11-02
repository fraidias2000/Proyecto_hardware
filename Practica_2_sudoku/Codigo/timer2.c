/*********************************************************************************************
* Fichero:		timer.c
* Autor:
* Descrip:		TIMER CONTOL LATIDO LEDS PLACA
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "timer2.h"
#include "44b.h"
#include "44blib.h"
#include "led.h"

/*--- variables globales ---*/
int VALOR_INICIAL_TIMER2 = 65535;
int variable_latido = 0;

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer2_ISR(void) __attribute__((interrupt("IRQ")));

// codigo de las funciones
void timer2_ISR(void)
{
	variable_latido = 1;
	// borrar bit en I_ISPC para desactivar la solicitud de interrupción
	rI_ISPC |= BIT_TIMER2;
}

void timer2_init(void)
{
	/* Configuraion controlador de interrupciones */
	rINTMOD &= ~(BIT_TIMER2); //Configura la linea del timer1 como IRQ
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK &= ~(BIT_TIMER2); // habilitamos en vector de mascaras de interrupcion el Timer1 (bits 26 y 12, BIT_GLOBAL y BIT_TIMER1 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER1 */
	pISR_TIMER2 = (unsigned) timer2_ISR;

	/* Configura el Timer1 */
	rTCFG0 = 0; // ajusta el preescalado
	rTCFG1 = 0x0; // selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2.
	rTCNTB2 = VALOR_INICIAL_TIMER2;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB2 = 12800;// valor de comparación

	//Ponemos manual update del timer 2 a 1 (bit 13)
	rTCON |=0x2000;
	//Activamos auto reload y start y desactivamos manual update, a la vez que respetamos los bits de timer 0 y 1
	//rTCON = 0x9909; //1001100100001001
	rTCON ^= 0xB000;
}
