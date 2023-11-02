
#ifndef _COLA_DEPURACION_H_
#define _COLA_DEPURACION_H_
#include "option.h"
#include <stdint.h>
#include "timer1.h"

/*--- declaracion de funciones visibles del módulo ---*/
void iniciar_cola_eventos();
void cola_guardar_eventos(uint8_t ID_evento, uint32_t auxData);
#endif /* PILA_DEPURACION_H */
