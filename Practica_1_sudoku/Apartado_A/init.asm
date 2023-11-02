
    .extern main
    .equ STACK, 0x0c7ff000
.text
.equ    num, 25  /* this is used to indicate the number of words to copy */
#        ENTRY                    /* mark the first instruction to call */
.arm /* indicates that we are using the ARM instruction set */
#------standard initial code
# --- Setup interrupt / exception vectors
.global    start
.extern     __c_copy_10
start:
       B     Reset_Handler
/* In this version we do not use the following handlers */
################################################################################
#-----------Undefined_Handler:
#      B        Undefined_Handler
#----------SWI_Handler:
#      B        SWI_Handler
#----------Prefetch_Handler:
#      B        Prefetch_Handler
#----------Abort_Handler:
#      B        Abort_Handler
#      NOP      /* Reserved vector */
#----------IRQ_Handler:
#      B        IRQ_Handler
#----------FIQ_Handler:
#      B        FIQ_Handler

################################################################################
# Reset Handler:
# the processor starts executing this code after system reset
################################################################################
Reset_Handler:
#
// src = direccion origen de los datos
// dst = direccion destino de los datos
        LDR     sp,=STACK       /*  set up stack pointer (r13) */

        LDR     r0, =src //Cargamos en r0 la direccion origen de los datos
        LDR     r1, =dst1 //Cargamos en r1 la direccion destino de los datos
        BX      ARM_copy_10   //Llamamos a la funcion arm

//Volvemos a cargar las direcciones porque al llamar a una funcion se han borrado
        LDR     r0, =src
        LDR     r1, =dst2
# function __c_copy is in copy.c

        LDR     r3, = __c_copy_10
        MOV     lr, pc
        BX      r3
stop:
        B       stop        /*  end of program */

################################################################################
# funci—n ARM:
# copia 25 palabras de la direcci—n indicada en r0 a la indicada por r1
# deber’a crear un marco de pila con la misma estructura que en el resto de llamadas a funciones
# pero como es un nodo hoja (no llama a ninguna funci—n) vamos a hacer un marco simplificado:
# s—lo guardamos los registros que utiliza y que no tiene permiso para alterar

//HAY QUE CAMBIAR LA MEMORIA PORQUE SI CARGAMOS MÁS DE 5 EN 5 NOS DA ERROR PORQUE OCUPA LA MEMORIA DE dist2
################################################################################
ARM_copy_10:
        push {r0-r11}
		MOV r2, #1
Loop:
		CMP r2, #4
  		BGE Done
        LDM r0!, {r3-r11} //Carga 9 valores de la fuente
        STM r1!, {r3-r11} //Escribimos 9 valores en el destino
		ADD r2, r2, #1
		B Loop
  Done:
        # restore the original registers
        pop {r0-r11}
        # return to the instruction that called the routine and to arm mode
        BX      r14
 ##########################################################################
ARM_copy_10:
        push {r0-r11}
	MOV r2, #1
Loop:
	CMP r2, #6
  	BGE Done
        LDM r0!, {r3-r11} //Carga 9 valores de la fuente
        STM r1!, {r3-r11} //Escribimos 9 valores en el destino
	ADD r2, r2, #1
	B Loop
  Done:		
        # restore the original registers
        pop {r0-r11}
        # return to the instruction that called the routine and to arm mode
        BX      r14
################################################################################
#        AREA BlockData, DATA, READWRITE
#    src:  source block (10 words)
#    dst1: destination block 1 (25 words)
#    dst2: destination block 2 (25 words)
################################################################################
.data
.ltorg /* guarantees the alignment */
src:
     .long     1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
dst1:
     .long     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
dst2:
     .long     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.end
#        END


