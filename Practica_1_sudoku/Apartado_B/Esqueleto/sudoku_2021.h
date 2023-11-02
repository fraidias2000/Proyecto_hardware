/* guarda para evitar inclusiones múltiples ("include guard") */
#ifndef SUDOKU_H_2021
#define SUDOKU_H_2021

#include <inttypes.h>
#include "celda.h"

/* Tamaños de la cuadrícula */
/* Se utilizan 16 columnas para facilitar la visualización */
enum {NUM_FILAS = 9,
      PADDING = 7,
      NUM_COLUMNAS = NUM_FILAS + PADDING};

/* Definiciones para valores muy utilizados */
enum {FALSE = 0, TRUE = 1};

typedef uint16_t CELDA;
/* La información de cada celda esta codificada en 16 bits
 * con el siguiente formato, empezando en el bit mas significativo (MSB):
 * 9 bits vector CANDIDATOS (0: es candidato, 1: no es candidato)
 * 1 bit  no usado
 * 1 bit  ERROR
 * 1 bit  PISTA
 * 4 bits VALOR
 */

/* declaración de funciones visibles en el exterior */

int sudoku9x9(CELDA cuadricula_C_C[NUM_FILAS][NUM_COLUMNAS],
	CELDA cuadricula_C_ARM[NUM_FILAS][NUM_COLUMNAS],
	CELDA cuadricula_ARM_ARM[NUM_FILAS][NUM_COLUMNAS],
	CELDA cuadricula_ARM_C[NUM_FILAS][NUM_COLUMNAS],
	CELDA solucion[NUM_FILAS][NUM_COLUMNAS]);



/* declaración de funciones ARM a implementar */

void
candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna);

int
candidatos_actualizar_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);

int
candidatos_actualizar_arm_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);

int
candidatos_propagar_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                               uint8_t fila, uint8_t columna);

#endif /* SUDOKU_H_2021 */
