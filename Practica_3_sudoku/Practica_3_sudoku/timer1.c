/*********************************************************************************************
* Fichero:		timer.c
* Autor:
* Descrip:		TIMER ENCARGADO DE MEDIR TIEMPOS (Normalmente para la cola de depuración)
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "timer1.h"
#include "44b.h"
#include "44blib.h"

/*--- variables globales ---*/
int timer1_num_int = 0;
int VALOR_INICIAL_TIMER1 = 65535;

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer1_ISR(void) __attribute__((interrupt("IRQ")));

// codigo de las funciones
void timer1_ISR(void)
{
	timer1_num_int ++; //Incrementamos la variable del contador para
	                   //saber que se ha producido una excepcion más
	// borrar bit en I_ISPC para desactivar la solicitud de interrupción
	rI_ISPC |= BIT_TIMER1; // BIT_TIMER1 está definido en 44b.h y pone
	                       // un uno en el bit 12 que correponde al Timer1
}

void timer1_init(void)
{
	/* Configuraion controlador de interrupciones */
	rINTMOD &= ~(BIT_TIMER1); //Configura la linea del timer1 como IRQ
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK &= ~(BIT_TIMER1); // habilitamos en vector de mascaras de
	                          // interrupcion el Timer1 (bits 26 y 12,
	                          // BIT_GLOBAL y BIT_TIMER1 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER1 */
	pISR_TIMER1 = (unsigned) timer1_ISR;

	/* Configura el Timer1 */
	rTCFG0 = 0; // ajusta el preescalado
	rTCFG1 = 0x0; // selecciona la entrada del mux que proporciona el reloj.
	              // La 00 corresponde a un divisor de 1/2.
	rTCNTB1 = VALOR_INICIAL_TIMER1;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB1 = 12800;// valor de comparación
	timer1_start();
}

void timer1_start()
{
	rTCNTO1 = rTCNTB1; // Inicializamos rTCNTO1 con el valor
	                   // inicial de cuenta (Reiniciamos la cuenta)
	/* establecer update=manual (bit 9)*/
	rTCON |= 0x200; // 001000001001
	/* establecer auto-reload (bit 11) e iniciamos timer (bit 8)*/
	//rTCON = 0x909; // 100100001001
	rTCON ^= 0xB00; // 100100000000
	timer1_num_int = 0;//Inicializamos la variable del timer
}
unsigned int timer1_count(){
	// calculamos el número de ciclos que se han ejecutado
	int ciclos_completos = timer1_num_int * (rTCNTB1 - rTCMPB1);
	//Calculamos cuanto nos queda del ciclo actual
	int ciclo_actual = rTCNTB1 - rTCNTO1;
	int ciclos_totales = ciclos_completos + ciclo_actual;
	return ciclos_totales/32;
}

unsigned int timer1_stop(){
	int tiempo = timer1_count();
	timer1_num_int = 0;
	/*Volvemos a configurar el timer estableciendo update=manual (bit 9)*/
	rTCON |= 0x200; // 001000001001
	rTCON ^= 0xB00; // Paramos el timer 1 (el timer 0 sigue funcionando)
	return tiempo;
}



