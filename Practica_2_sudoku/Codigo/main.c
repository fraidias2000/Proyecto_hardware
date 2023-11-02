/*********************************************************************************************
* Fichero:	main.c
* Autor:
* Descrip:	punto de entrada de C
* Version:  <P4-ARM.timer-leds>
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "button.h"
#include "led.h"
#include "timer0.h"
#include "timer1.h"
#include "timer2.h"
#include "cola.h"
#include "44blib.h"
#include "44b.h"


#include "sudoku_2021.h"
#include "eventos.h"

void iniciarDispositivos();

/*--- codigo de funciones ---*/
void Main(void)
{
	iniciarDispositivos();
	timer1_start();

	/*cola_guardar_eventos(INICIO_SUDOKU,C_ARM);
	iniciar_sudoku();
	cola_guardar_eventos(FIN_SUDOKU,C_ARM);
	*/
	while (1)
	{
		// Cambia los leds con cada interrupcion del temporizador */
		if (variable_latido == 1){
			latido_led1();
			variable_latido = 0;

		}
	}
}

void iniciarDispositivos(){
//Inicializacion de la placa, interrupciones y puertos
	sys_init();

// Inicializacion de los temporizadores
	iniciarTimers();

// Inicializamos los pulsadores. Cada vez que se pulse se verá reflejado en el 8led
	Eint4567_init();

// Inicializamos la cola de depuracion
	iniciar_cola_eventos();

// Inicializamos el 8led
	D8Led_init();

//  Inicializamos el tablero del sudoku
	iniciar_sudoku();
}

void iniciarTimers(){
	timer0_init();
	timer1_init();
	timer2_init();
}
