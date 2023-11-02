#include <stddef.h>
#include "sudoku_2021.h"

/* *****************************************************************************
 * propaga el valor de una determinada celda
 * para actualizar las listas de candidatos
 * de las celdas en su su fila, columna y region */
/* Recibe como par‡metro la cuadr’cula, y la fila y columna de
 * la celda a propagar; no devuelve nada
 */
void candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna)
{
	uint8_t j, i , init_i, init_j, end_i, end_j;
    	/* puede ayudar esta "look up table" a mejorar el rendimiento */
    	const uint8_t init_region[NUM_FILAS] = {0, 0, 0, 3, 3, 3, 6, 6, 6};

    /* valor que se propaga */
    uint8_t valor = celda_leer_valor(cuadricula[fila][columna]);

    /* recorrer fila descartando valor de listas candidatos */

    /* recorrer columna descartando valor de listas candidatos */

    /* recorrer region descartando valor de listas candidatos */
}

/* *****************************************************************************
 * calcula todas las listas de candidatos (9x9)
 * necesario tras borrar o cambiar un valor (listas corrompidas)
 * retorna el numero de celdas vac’as */
static int candidatos_actualizar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
    //int celdas_vacias = 0;
    // uint8_t i;
    // uint8_t j;


    //borrar todos los candidatos
	//...
	//recalcular candidatos de las celdas vacias calculando cuantas hay vacias
	//...
	//retornar el numero de celdas vacias
	//...
	return -1; // por favor eliminar una vez completada la funci—n
}


/* Init del sudoku en c—digo C invocando a propagar en arm
 * Recibe la cuadr’cula como primer par‡metro
 * y devuelve en celdas_vacias el nœmero de celdas vac’as
 */

static int
candidatos_actualizar_c_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
	return -1; // por favor eliminar una vez completada la funci—n
}


static int
cuadricula_candidatos_verificar(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
	 CELDA solucion[NUM_FILAS][NUM_COLUMNAS])
{
    int correcto = 1;
    uint8_t i;
    uint8_t j;

	return -1; // por favor eliminar una vez completada la funci—n
    
}


/* *****************************************************************************
 * Funciones pœblicas
 * (pueden ser invocadas desde otro fichero) */

/* *****************************************************************************
 * programa principal del juego que recibe el tablero */

int
sudoku9x9(CELDA cuadricula_C_C[NUM_FILAS][NUM_COLUMNAS],
	CELDA cuadricula_C_ARM[NUM_FILAS][NUM_COLUMNAS],
	CELDA cuadricula_ARM_ARM[NUM_FILAS][NUM_COLUMNAS],
	CELDA cuadricula_ARM_C[NUM_FILAS][NUM_COLUMNAS],
	CELDA solucion[NUM_FILAS][NUM_COLUMNAS])
{
    int celdas_vacias[4];    //numero de celdas aun vac’as
    int correcto = 0;
    size_t i;

    /* calcula lista de candidatos, versi—n C */
    celdas_vacias[0] = candidatos_actualizar_c(cuadricula_C_C);

    /* A–adir para el resto de "versiones" */

 	for (i=1; i < 4; ++i) {
		if (celdas_vacias[i] != celdas_vacias[0]) {
		return -1;
		}
	}

    /* verificar que la lista de candidatos calculada es correcta */
    /* cuadricula_candidatos_verificar(...) */

}

