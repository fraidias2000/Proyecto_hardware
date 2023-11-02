/*--- ficheros de cabecera ---*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "cola.h"
#include "44blib.h"
#include "44b.h"
#include "timer1.h"
#include "option.h"
#include "def.h"

/*--- Variables globales ---*/
/*static uint32_t *cola_eventos; //Puntero de cola, debe ser static para que se quede en memoria
static volatile int totalElementos;
//static volatile uint32_t * base_cola;
static uint32_t inicio_cola = (_ISR_STARTADDRESS - 0xf00) - 256; //256*3; //(_ISR_STARTADDRESS - 0xf00) = 0xc7ff000
static uint32_t EspacioMax = 256;
uint32_t iterador = 0; //Iterador de la cola
*/
/*CONSTANTES*/
#define ESPACIO_MAX 768 //256*3
#define INICIO_COLA (_ISR_STARTADDRESS-0xf00) - ESPACIO_MAX

/*VARIABLES*/
static uint32_t *cola = NULL; //Debe ser static para que se almacene en memoria
uint32_t iterador;
uint32_t totalElementos;


/*--- Funciones del fichero ---*/

// Inicia la cola de depuración
void iniciar_cola_eventos() {
	//_ISR_STARTADDRESS esta definido en opcion.h
	cola = (uint32_t*) INICIO_COLA;
	totalElementos = 0;

	//Limpiamos la cola
	int i;
	for (i = 0; i < ESPACIO_MAX; i++) {
			cola[i] = 0;
		}
}

// Apilar elementos en la pila
void cola_guardar_eventos(uint8_t ID_evento, uint32_t auxData)
{
	int tiempo_evento = timer1_count();
	uint32_t dato_evento = (uint32_t) ID_evento << 24;//dato_evento era un entero de 8 bits,
	                                                  //debemos pasarlo a 32 bits para que se
	                                                 //almacene alineado en la cola
	// Si la cola llega al limite se reinicia
	if(iterador  == ESPACIO_MAX) {
		iterador = 0;
	}

	uint32_t direccion = (uint32_t*) cola;
	// Apilamos los datos
	 cola[iterador] = ID_evento; //Guardamos el tipo de evento
	 cola[++iterador] = auxData; //Guardamos exactamente que interrupcion ha ocurrido
	 cola[++iterador] = tiempo_evento; // Ponemos ++iterador y no iterador++
	                                   // para que aumente antes de acceder a la posicion

	 iterador++;
	totalElementos++;
}

