/*********************************************************************************************
* Fichero:	button.c
* Autor:
* Descrip:	Funciones de manejo de los pulsadores (EINT6-7)
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "44blib.h"
#include "44b.h"
#include "cola.h"
#include "eventos.h"
#include "def.h"
#include "maquina_estados.h"



/*--- variables globales del módulo ---*/
//Variables para sudoku
uint8_t fila = 1;
uint8_t columna = 1;
uint8_t valor = 0;


int BOTON_IZQUIERDO = 179;//10110011
int BOTON_DERECHO = 115; //01110011
int NINGUN_BOTON = 243; //11110011

int estado_rebotes = ESTADO_INICIAL;
int estado_sudoku = ESTADO_SIN_JUGAR;
int boton_pulsado = 0;

int es_segunda_vez_filas = 0;
int es_segunda_vez_columnas = 0;
int es_segunda_vez_valor = 0;


int boton_mantenido = 0; //Se usa para que solo se ejecute el metodo "timer0_boton_mantenido" una vez ya que si se ejecuta dos se sobreescribe el autoreload

int valor_cambiado = 0;  //0 = NO, 1 = SI
//Metodo que representa la maquina de estados para evitar los rebotes.
void maquina_estado_rebotes(){
	switch(estado_rebotes){
		case ESTADO_INICIAL:
			if((rPDATG  == BOTON_IZQUIERDO) || (rPDATG  == BOTON_DERECHO)){ //Se esta pulsado uno de los dos botones
				estado_rebotes = ESTADO_PULSADO;
				if(boton_mantenido == 0){ //Se hace para que solo se ejecute una vez
					timer0_boton_mantenido();
					boton_mantenido ++;
				}
			}else if (rPDATG == NINGUN_BOTON){ //No se esta pulsando ningun boton
				estado_rebotes = ESTADO_FINAL;
				timer0_trd();//Delay despulsar
			}
			break;
		case ESTADO_PULSADO: //Hace lo mismo que ESTADO_INICIAL
			if((rPDATG  == BOTON_IZQUIERDO) || (rPDATG  == BOTON_DERECHO)){ //Se esta pulsado uno de los dos botones
				estado_rebotes = ESTADO_PULSADO;
				if(boton_mantenido == 0){ //Se hace para que solo se ejecute una vez
					timer0_boton_mantenido();
					boton_mantenido ++;
				}
			}else if (rPDATG == NINGUN_BOTON){ //No se esta pulsando ningun boton
				estado_rebotes = ESTADO_FINAL;
				timer0_trd(); //Delay despulsar
			}
			break;
		case ESTADO_FINAL:
			boton_mantenido = 0;
			timer0_stop(); //Paramos el timer 0 para que no vuelva a saltar
			if(estado_sudoku == ESTADO_SIN_JUGAR){//Se evalua que sea la primera vez que se pulsa el boton
				estado_sudoku = ESTADO_INICIAL_JUEGO;
			}
			maquina_estados_sudoku2021();
			break;
	}
}

void maquina_estados_sudoku2021(){
	switch(estado_sudoku){
	case ESTADO_INICIAL_JUEGO:
		D8Led_symbol(15); //Ponemos la F en el dispay
		estado_sudoku = ESTADO_FILA;
		fila = 1;
		columna = 1;
		valor = 0;
		es_segunda_vez_filas = 0;
		es_segunda_vez_columnas = 0;
		es_segunda_vez_valor = 0;
		break;
	case ESTADO_FILA:
		D8Led_symbol(fila);
		 // Lo usamos para que la primera vez que nos metemos en las filas, podamos leer el 1 y no se incremente directamente a 2
			if(boton_pulsado == DERECHO){
				if(es_segunda_vez_filas == 1){
					if(fila == 9){
						fila = 1;
					}else{
						fila++;
					}
					D8Led_symbol(fila & 0x000f);
				}
				es_segunda_vez_filas = 1;
				}else if(boton_pulsado  == IZQUIERDO){
					estado_sudoku = ESTADO_COLUMNA;
					D8Led_symbol(12); //Ponemos la C en el dispay
				}
			break;
	case ESTADO_COLUMNA:
		if(boton_pulsado == DERECHO){
			if(es_segunda_vez_columnas == 1){
				if(columna == 9){
					columna = 1;
				}else{
					columna++;
				}
				D8Led_symbol(columna & 0x000f);
				}
				es_segunda_vez_columnas = 1;
			}else if(boton_pulsado  == IZQUIERDO){
				estado_sudoku = ESTADO_VALOR;
				D8Led_symbol(12);
			}
		break;
	case ESTADO_VALOR:
		if(pasarela_celda_es_pista(fila, columna, valor) == 1 ){ // Si es = 1 será porque es pista
		   estado_sudoku = ESTADO_INICIAL_JUEGO;
		   maquina_estados_sudoku2021();
		}else{ //si no es pista
			if(boton_pulsado == DERECHO){
			    if(es_segunda_vez_valor == 1){
				    if(valor == 9){
				        valor = 0;
					}else{
					    valor++;
					}
				    D8Led_symbol(valor & 0x000f);
				    es_segunda_vez_valor = 1;
				   }
			}else if(boton_pulsado  == IZQUIERDO){
				D8Led_symbol(12);
				estado_sudoku = ESTADO_INICIAL_JUEGO;
			   }
			if(pasarela_celda_comprobar_candidato(fila,columna,valor) == 1){ //Se comprueba si el valor está en la lista de candidatos
				if(pasarela_celda_leer_valor(fila, columna)!= 0){ //Si es distinto de 0 es porque ya había un valor
					valor_cambiado = 1;
				}
		       	D8Led_symbol(10); //Pondrá A de acierto en el display
		       	pasarela_poner_valor_celda(fila, columna, valor);
		       	pasarela_propagar_valor( fila, columna, valor);

		       	if((valor == 0)|| (valor_cambiado == 1)){ //En el caso de que se quiera borrar una celda o se haya sobreescrito una celda que no sea pista
		        	pasarela_actualizar();
		        	valor_cambiado = 0;
		        }
			}else{
				D8Led_symbol(E);
			}
		}
		break;
	}
}
