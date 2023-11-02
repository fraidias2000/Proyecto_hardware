/*********************************************************************************************
* Fichero:	button.c
* Autor:
* Descrip:	Funciones de manejo de los pulsadores (EINT6-7)
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "button.h"
#include "8led.h"
#include "44blib.h"
#include "44b.h"
#include "cola.h"
#include "eventos.h"
#include "maquina_estados.h"


static unsigned int numeroLED = 0;
/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void Eint4567_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de funciones ---*/
void Eint4567_ISR(void)
{
	/* Identificar la interrupcion (hay dos pulsadores)*/
	rINTMSK |= BIT_EINT4567; // Desactivar interrupciones botones

	switch (rEXTINTPND)
			{
				case 4:
					numeroLED = 4;
					//D8Led_symbol(numeroLED & 0x000f); // sacamos el valor por pantalla (módulo 16)
					cola_guardar_eventos(BOTON,IZQUIERDO);
					boton_pulsado = IZQUIERDO;
					break;
				case 8:
					numeroLED = 8;
					//D8Led_symbol(numeroLED & 0x000f); // sacamos el valor por pantalla (módulo 16)
					cola_guardar_eventos(BOTON,DERECHO);
					boton_pulsado = DERECHO;
					break;
				default:
					break;
			}
	estado_rebotes = ESTADO_INICIAL;
	timer0_trp(); //Delay pulsar

	/* Finalizar ISR */
	rEXTINTPND = 0xf;				// borra los bits en EXTINTPND
	rI_ISPC   |= BIT_EINT4567;		// borra el bit pendiente en INTPND
	rINTMSK &= ~(BIT_EINT4567); // Habilitar nuevamente las interrupciones del boton

}

void Eint4567_init(void)
{
	/* Configuracion del controlador de interrupciones. Estos registros están definidos en 44b.h */
	rI_ISPC    = 0x3ffffff;	// Borra INTPND escribiendo 1s en I_ISPC
	rEXTINTPND = 0xf;       // Borra EXTINTPND escribiendo 1s en el propio registro
	rINTMOD    = 0x0;		// Configura las linas como de tipo IRQ
	rINTCON    = 0x1;	    // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK    &= ~(BIT_EINT4567); // habilitamos interrupcion linea eint4567 en vector de mascaras

	/* Establece la rutina de servicio para Eint4567 */
	pISR_EINT4567 = (int) Eint4567_ISR;

	/* Configuracion del puerto G */
	rPCONG  = 0xffff;        		// Establece la funcion de los pines (EINT0-7)
	rPUPG   = 0x0;                  // Habilita el "pull up" del puerto
	rEXTINT = rEXTINT | 0x22222222;   // Configura las lineas de int. como de flanco de bajada

	/* Por precaucion, se vuelven a borrar los bits de INTPND y EXTINTPND */
	rEXTINTPND = 0xf;				// borra los bits en EXTINTPND
	rI_ISPC   |= BIT_EINT4567;		// borra el bit pendiente en INTPND
}

