
#ifndef _MAQUINA_ESTADOS_H_
#define _MAQUINA_ESTADOS_H_

#define FILA  F
#define COLUMNA C
enum estados_rebotes{
	ESTADO_INICIAL,
	ESTADO_PULSADO,
	ESTADO_FINAL
};

enum estados_sudoku_2021{
	ESTADO_SIN_JUGAR,
	ESTADO_INICIAL_JUEGO,
	ESTADO_FILA,
	ESTADO_COLUMNA,
	ESTADO_VALOR
};

//Definicion de los botones en el registro PDATG
int BOTON_IZQUIERDO;
int BOTON_DERECHO;
int NINGUN_BOTON;

int estado_rebotes;
int estado_sudoku;
int boton_pulsado;

int es_segunda_vez_filas;
int es_segunda_vez_columnas;
int es_segunda_vez_valor;

int boton_mantenido; //Se usa para que solo se ejecute el metodo "timer0_boton_mantenido" una vez ya que si se ejecuta dos se sobreescribe el autoreload

int valor_cambiado;  //0 = NO, 1 = SI

void maquina_estado_rebotes();
void maquina_estados_sudoku2021();
#endif /* _MAQUINA_ESTADOS_H_ */
