/* guarda para evitar inclusiones m√∫ltiples (include guard) */
#ifndef CELDA_H
#define CELDA_H

#include <inttypes.h>

/* Cada celda se codifica en 16 bits
 * bits [15,7]: los 9 bits m√°s significativos representan el vector de candidatos,
 * si el bit 7 + valor - 1 est√° a 0, valor es candidato, 1 en caso contrario
 * bit 6: no empleado
 * bit 5: error
 * bit 4: pista
 * bits [3,0]: valor de la celda
 */

enum { BIT_CANDIDATOS = 7 };

typedef uint16_t CELDA;

/* *****************************************************************************
 * elimina el candidato del valor almacenado en la celda indicada */
__inline  void celda_eliminar_candidato(CELDA *celdaptr, uint8_t valor)
{
    *celdaptr = *celdaptr | (0x0001 <<	 (BIT_CANDIDATOS + valor - 1));
}

/* *****************************************************************************
 * modifica el valor almacenado en la celda indicada */
__inline static void
celda_poner_valor(CELDA *celdaptr, uint8_t val)
{
    *celdaptr = (*celdaptr & 0xFFF0) | (val & 0x000F);        //*celdaptr & 0xFFF0 = Pone el valor a 0   ; val & 0x000F = Borra todo menos el valor
}

/* *****************************************************************************
 * extrae el valor almacenado en los 16 bits de una celda */
__inline  uint8_t
celda_leer_valor(CELDA celda)
{
    return (celda & 0x000F);
}


__inline static void celda_eliminar_todos_candidatos(CELDA *celdaptr)
{
	*celdaptr = *celdaptr | 0xFF80;  // FF80 = 1111 1111 1000 0000 Al sumar este n˙mero, todos los candidatos se eliminan (se ponen a 1) y los dem·s valores se quedan igual
}
#endif // CELDA_H
