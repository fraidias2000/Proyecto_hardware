#include <stddef.h>
#include "sudoku_2021.h"
#include "tableros.h"
/* *****************************************************************************
 * propaga el valor de una determinada celda
 * para actualizar las listas de candidatos
 * de las celdas en su su fila, columna y region */
/* Recibe como par�metro la cuadr�cula, y la fila y columna de
 * la celda a propagar; no devuelve nada
 */
void candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna, uint8_t valor )
{
	const uint8_t inicio_bloque[3]={0,3,6};
	uint8_t fila_inicio = 0;
    uint8_t columna_inicio = 0;
    uint8_t reset_columnas = 0; //Sirve para reiniciar el valor de la columna para volver a recorrer las columnas de la siguiente fila
    uint16_t fila_final_region = 0;
    uint16_t columna_final_region = 0;


    //recorrer fila descartando valor de listas candidatos
    while(columna_inicio < NUM_FILAS){ //No se usa(j<NUM_COLUMNAS) porque hay m�s columnas que filas debido al padding
    	celda_eliminar_candidato(&cuadricula[fila][columna_inicio], valor);
   		columna_inicio++;
    }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

     //recorrer columna descartando valor de listas candidatos
    while(fila_inicio < NUM_FILAS){ //No se usa(j<NUM_COLUMNAS) porque hay m�s columnas que filas debido al padding
    	celda_eliminar_candidato(&cuadricula[fila_inicio][columna], valor);
    	fila_inicio++;
       }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     //recorrer region descartando valor de listas candidatos
    //Se averigua en que cuadrante nos encontramos
    if(fila < 3){                          //Fila se encuentra en el primer cuadrante
    	fila_inicio = inicio_bloque[0];
    } else if((fila >=3 ) &&(fila < 6)){   //Fila se encuentra en el segundo cuadrante
    	fila_inicio = inicio_bloque[1];
    }else{                                 //Fila se encuentra en el tercer cuadrante
    	fila_inicio = inicio_bloque[2];
    }

    if(columna < 3){                          //Columna se encuentra en el primer cuadrante
    	columna_inicio = inicio_bloque[0];
    }else if((columna >=3 ) &&(columna < 6)){   //Columna se encuentra en el segundo cuadrante
    	columna_inicio = inicio_bloque[1];
    }else{                                 //Columna se encuentra en el tercer cuadrante
    	columna_inicio = inicio_bloque[2];
    }

    fila_final_region = fila_inicio + 3;
    columna_final_region = columna_inicio + 3;
    reset_columnas = columna_inicio;

    //Se recorre la region
    while(fila_inicio < fila_final_region){ //Se recorren las filas
    	while(columna_inicio < columna_final_region){ //Se recorren las columnas
    		celda_eliminar_candidato(&cuadricula[fila_inicio][columna_inicio], valor);
    		columna_inicio ++;
    	}
    	columna_inicio = reset_columnas;
    	fila_inicio++;
    }
}
/* *****************************************************************************
 * calcula todas las listas de candidatos (9x9)
 * necesario tras borrar o cambiar un valor (listas corrompidas)
 * retorna el numero de celdas vacias */

//EJECUTA ACTUALIZAR EN C Y PROPAGAR EN C
static int candidatos_actualizar_c_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
    int celdas_vacias = 0;
    int i = 0;
    int j;
    uint8_t valor = 0;
	while(i<NUM_FILAS){
		j = 0;
		while(j<NUM_FILAS){ //No se (j<NUM_COLUMNAS) porque hay m�s columnas que filas debido al padding
			valor = celda_leer_valor(cuadricula[i][j]);
			if(valor == 0){
				//actualizar contador de celdas vacias
				celdas_vacias++;
			}else{ //Si es distinto de 0 es porque hay un valor y debe ser propagado
				candidatos_propagar_c(cuadricula,i,j, valor);
			}
			j++;
		}
		i++;
	}
    //retornar el numero de celdas vacias
    return celdas_vacias;
}

/* Init del sudoku en c�digo C invocando a propagar en arm
 * Recibe la cuadr�cula como primer par�metro
 * y devuelve en celdas_vacias el n�mero de celdas vac�as
 */

//EJECUTA ACTUALIZAR EN C Y PROPAGAR EN ARM
int
candidatos_actualizar_c_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
	int celdas_vacias = 0;
	    int i = 0;
	    int j;
	    uint8_t valor = 0;

	    //borrar todos los candidatos
		while(i<NUM_FILAS){
			j = 0;
			while(j<NUM_FILAS){ //No se (j<NUM_COLUMNAS) porque hay m�s columnas que filas debido al padding
				valor = celda_leer_valor(cuadricula[i][j]);
				if(valor == 0){
					//actualizar contador de celdas vacias
					celdas_vacias++;
				}else{ //Si es distinto de 0 es porque hay un valor y debe ser propagado
					candidatos_propagar_arm(cuadricula,i,j, valor); //Llamamos a propagar en ARM
				}
				j++;
			}
			i++;
		}
	    //retornar el numero de celdas vacias
	    return celdas_vacias;
}

/* *****************************************************************************
 * Funciones p�blicas
 * (pueden ser invocadas desde otro fichero) */

/* *****************************************************************************
 * programa principal del juego que recibe el tablero */
void
sudoku9x9(CELDA cuadricula_C_C[NUM_FILAS][NUM_COLUMNAS])
{
    candidatos_actualizar_c_arm(cuadricula_C_C);

}
int iniciar_sudoku(void)
{
	sudoku9x9(cuadricula_C_C);
}

//PASARELAS LLAMADAS DESDE BOTON
uint8_t pasarela_celda_es_pista(uint8_t fila, uint8_t columna, uint8_t valor){
	return celda_es_pista(cuadricula_C_C[fila][columna], valor);
}


void pasarela_actualizar(){
	candidatos_actualizar_c_arm(cuadricula_C_C);
}


void pasarela_poner_valor_celda(uint8_t fila, uint8_t columna, uint8_t valor){
	celda_poner_valor(&cuadricula_C_C[fila][columna], valor);
}

void pasarela_propagar_valor(uint8_t fila, uint8_t columna, uint8_t valor){
	candidatos_propagar_arm(cuadricula_C_C,fila,columna, valor);
}

uint8_t pasarela_celda_leer_valor(uint8_t fila, uint8_t columna){
	return celda_leer_valor(cuadricula_C_C[fila][columna]);
}

int pasarela_celda_comprobar_candidato (uint8_t fila, uint8_t columna, uint8_t valor){
	return celda_comprobar_candidato(cuadricula_C_C[fila][columna], valor);

}




