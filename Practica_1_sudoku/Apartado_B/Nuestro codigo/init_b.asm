.text

#        ENTRY                              /*  mark the first instruction to call */
.global start
.global candidatos_propagar_arm
.global candidatos_actualizar_c_arm
.global candidatos_actualizar_all
.global candidatos_actualizar_arm_arm
.global candidatos_actualizar_arm_c

start:
.arm    /* indicates that we are using the ARM instruction set */


#------standard initial code
# --- Setup interrupt / exception vectors
      B       Reset_Handler
/* In this version we do not use the following handlers */
################################################################################
#-----------Undefined_Handler:
#      B       Undefined_Handler
#----------SWI_Handler:
#      B       SWI_Handler
#----------Prefetch_Handler:
#      B       Prefetch_Handler
#----------Abort_Handler:
#      B       Abort_Handler
#         NOP      /* Reserved vector */
#----------IRQ_Handler:
#      B       IRQ_Handler
#----------FIQ_Handler:
#      B       FIQ_Handler

################################################################################
# Reset Handler:
# the processor starts executing this code after system reset
################################################################################
Reset_Handler:
#
        MOV     sp, #0x4000 //set up stack pointer (r13)
#
#  USING A .C FUNCTION
#
# FUNCTION CALL the parameters are stored in r0 and r1
# If there are 4 or less parameters when calling a C function the compiler
# assumes that they have been stored in r0-r3.
# If there are more parameters you have to store them in the data stack
# using the stack pointer
.extern main
    	ldr r5, =main
    	mov lr,pc
    	bx r5
stop:
        B       stop        /*  end of program */

################################################################################
candidatos_propagar_arm: // Propaga los candidatos en ARM
    STMFD sp!, {r0-r9,lr}
    //r0 = cuadricula
    //r1 = Fila
    //r2 = columna
    //r3 = valor
    //r4 = iterador columna
    //r5 = NUM_COLUMNAS/FILAS
    mov r4, #0
    mov r5, #9
    mov r1, r1, LSL #5 // Calculamos cual es la fila donde tenemos que propagar (Filas* 32)
    add r0, r0, r1 //Sumamos a la direccion inicial la fila donde vamos a empezar a propagar
    //RECORRER FILA DESCARTANDO VALOR DE LISTAS CANDIDATOS
	B testBucleColumnas
	bucleColumnas:
		ldr r1, [sp, #12] //Cargamos el valor de celda_leer_valor(cuadricula[fila][columna]) como segundo parametro de la funcion "celda_eliminar_candidato"
    	bl celda_eliminar_candidato
    	add r0, r0, #2 //Nos deplazamos por las columnas de la fila
		add r4, r4, #1 //columna_inicio++
	testBucleColumnas:
	cmp r4, r5
	bne bucleColumnas
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	// RECORRER COLUMNA DESCARTANDO VALOR DE LISTAS CANDIDATOS
	//r0 = direccion tablero
	//r1 = fila
	//r2 = columna
	//r3 = registro auxiliar
	//r4 = columna_inicio
	//r5 = NUM_COLUMNAS/FILAS
	 mov r4, #0 //Reseteamos el iterador
	 ldr r0, [sp] //Cargamos en r0 la direccion inicial del tablero
	 ldr r2, [sp, #8] //Cargamos en r2 el número de columnas desde la pila
	 mov r3, r2, LSL #1 // Calculamos cual es la columna donde tenemos que propagar (columnas * 2)
     add r0, r0, r3 //Sumamos a la direccion inicial la columna donde vamos a empezar a propagar
	 B testBucleFilas
	bucleFilas:
		ldr r1, [sp, #12] //Cargamos el valor de celda_leer_valor(cuadricula[fila][columna]) como segundo parametro de la funcion "celda_eliminar_candidato"
    	bl celda_eliminar_candidato
    	add r0, r0, #32 //Nos deplazamos por de la filas
		add r4, r4, #1 //columna_inicio++
	testBucleFilas:
	cmp r4, r5
	bne bucleFilas
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------
	// RECORRER REGION DESCARTANDO VALOR DE LISTAS CANDIDATOS
	//r0 = direccion tablero
    //r1 = fila (i)
    //r2 = columna (j)
    //r3 = auxiliar multiplicacion
	//r4 = fila inicio cuadrante
	//r5 = columna inicio cuadrante
    //r6 = fila final region
	//r7 = columna final region
	//r8 = auxiliar  multiplicacion
	//r9 =  auxiliar suma

	//Se averigua en que cuadrante nos encontramos
 	//CALCULAMOS LA FILA INICIAL DEL CUADRANTE
    ldr r1, [sp, #4] //Cargamos en r1 la fila donde estamos
    cmp r1, #3 //if(fila < 3)
	movlt r4, #0 //less than

	cmp r1, #6   //if (fila < 6)
	cmplt r1, #3 //&& (fila >=3 )
	movge r4, #3

	cmp r1, #6 //if(fila => 6)
	movge r4, #6

	//CALCULAMOS LA COLUMNA INICIAL DEL CUADRANTE
	ldr r2, [sp, #8] //Cargamos en r2 la columna donde estamos
    cmp r2, #3 //if(columna < 3)
	movlt r5, #0 //less than

	cmp r2, #6   //if (columna < 6)
	cmplt r2, #3 //&& (columna >=3 )
	movge r5, #3

	cmp r2, #6 //if(columna => 6)
	movge r5, #6

	add r6, r4, #3 //fila_final_region = fila_inicio + 3;
	add r7, r5, #3 //columna_final_region = columna_inicio + 3;

	STMFD sp!, {r5} //Guardamos en la pila la columna inicio para resetear
    B testBucleFilasGrande
	bucleFilasGrande:
		B testBucleColumnasPequenio
		bucleColumnasPequenio:
	    	ldr r1, [sp, #16] //Cargas en r1 el valor
    		ldr r0, [sp,#4] //Cargamos en r0 la direccion inicial del tablero
    		mov r3, r4, LSL #5  //Multiplicamos por 32 el registro 7 (fila inicio cuadrante)
			mov r8, r5, LSL #1 //Multiplicamos por 2 el registro 8 (columna inicio cuadrante)
			add r3, r8, r3 //Sumamos la posición fila cuadrante + posicion columna cuadrante
			add r0, r0, r3 //Direccin inicio region = Direccion inicio + (filas* 32) + (columnas * 2)
			bl celda_eliminar_candidato
    		add r5, r5, #1 // columna_inicio ++
		testBucleColumnasPequenio:
		cmp r5, r7
		bne bucleColumnasPequenio
	add r4, r4, #1 //fila_inicio++
	ldr r5, [sp] // Cargamos de la pila columna inicio para resetearla
	testBucleFilasGrande:
	cmp r4, r6 //Compara las filas con el final de la region
	bne bucleFilasGrande
	add sp, sp, #4 //Sumamos posicion al puntero de pila para "eliminar" el valor que hemos metido en la pila durante la ejecucion de esta subrutina
	LDMFD sp!, {r0-r9, lr}
    mov pc, lr

################################################################################
//EJECUTA ACTUALIZAR EN ARM Y PROPAGAR EN C
candidatos_actualizar_arm_c:
    STMFD sp!, {r0-r7,lr}
    //r0 = direccion tablero
    //r1,r2,r3 //Se usan como registros auxiliares y como parametros de entrada-salida
    mov r4, #0 //int celdas_vacias = 0;
    mov r5, #0 //int i = 0;
	mov r6, #0 //int j;
	mov r7, #9 //Tamaño tablero
    B testBucleActualizarGrande
	bucleActualizarGrande:
		B testBucleActualizarPequenio
		bucleActualizarPequenio:
			//valor = celda_leer_valor(cuadricula[i][j]);
    		ldr r0, [sp] // cargamos en el registro 0 la dirección de inicio de la cuadricula
			mov r1, r5, LSL #5 //i * 32
			mov r2, r6, LSL #1 // j * 2
    		add r3, r1, r2   // DESPLAZAMIENTO
    		ldr r0, [r0,r3] //Cargamos el valor en r0 para llamar a la función "celda_leer_valor"
			bl celda_leer_valor
			cmp r0, #0 //if(valor == 0)
			beq celdasVaciasARM //Saltamos al final del bucle para no llamar a propagar
			mov r3, r0 //cargamos en r3 el valor
			ldr r0, [sp] // cargamos en r0 la direccion de la cuadricula
			mov r1, r5 // cargamos en r1 la variable i
			mov r2, r6 // cargamos en r2 la variable j
			bl candidatos_propagar_c //candidatos_propagar_c(cuadricula,i,j);
			finalBucleARM:
			add r6, r6, #1 //j++
		testBucleActualizarPequenio:
		cmp r6, r7 //Comparamos columnas con el final de tablero
		bne bucleActualizarPequenio
		add r5, r5, #1 //i++
		mov r6, #0 //j = 0;
	testBucleActualizarGrande:
	cmp r5, r7 //Compara las filas con el final de tablero
	bne bucleActualizarGrande
	add sp, sp, #4 //No queremos restaurar de pila r0 porque queremos retornar el número de celdas, por eso avanzamos a la siguiente instruccion
    mov r0, r4 //retornamos numero celdas vacias
	LDMFD sp!, {r1-r7,lr}
	mov pc, lr
celdasVaciasARM:
addeq r4, r4, #1 // celdas_vacias++;
b finalBucleARM //Vamos al final del bucle para volver a iterar
################################################################################
//EJECUTA ACTUALIZAR EN ARM Y PROPAGAR EN ARM
candidatos_actualizar_arm_arm:
    STMFD sp!, {r0-r7,lr}
    //r0 = direccion tablero
    //r1,r2,r3 //Se usan como registros auxiliares y como parametros de entrada-salida
    mov r4, #0 //int celdas_vacias = 0;
    mov r5, #0 //int i = 0;
	mov r6, #0 //int j;
	mov r7, #9 //Tamaño tablero

    B testBucleActualizarGrandeARM
	bucleActualizarGrandeARM:
		B testBucleActualizarPequenioARM
		bucleActualizarPequenioARM:
			//valor = celda_leer_valor(cuadricula[i][j]);
    		ldr r0, [sp] // cargamos en el registro 0 la dirección de inicio de la cuadricula
			mov r1, r5, LSL #5 //i * 32
			mov r2, r6, LSL #1 // j * 2
    		add r3, r1, r2   // DESPLAZAMIENTO
    		ldr r0, [r0,r3] //Cargamos el valor en r0 para llamar a la función "celda_leer_valor"
			bl celda_leer_valor
			cmp r0, #0 //if(valor == 0)
			addeq r4, r4, #1 // celdas_vacias++;
			ldrne r0, [sp] // cargamos en r0 la direccion de la cuadricula
			movne r1, r5 // cargamos en r1 la variable i
			movne r2, r6 // cargamos en r2 la variable j
			bl candidatos_propagar_arm //candidatos_propagar_arm(cuadricula,i,j);
			add r6, r6, #1 //j++
		testBucleActualizarPequenioARM:
		cmp r6, r7 //Comparamos columnas con el final de tablero
		bne bucleActualizarPequenioARM
		add r5, r5, #1 //i++
		mov r6, #0 //j = 0;
	testBucleActualizarGrandeARM:
	cmp r5, r7 //Compara las filas con el final de tablero
	bne bucleActualizarGrandeARM

    add sp, sp, #4 //No queremos restaurar de pila r0 porque queremos retornar el número de celdas, por eso avanzamos a la siguiente instruccion
    mov r0, r4 //retornamos numero celdas vacias
	LDMFD sp!, {r1-r7,lr}
	mov pc, lr
################################################################################
candidatos_actualizar_all: //Ejecuta en una misma funcion candidatos_actualizar_arm y candidatos_propagar_arm
    STMFD sp!, {r0-r11,lr}
    //Explicaremos la función de cada registro dependiendo de la parte del código donde nos encontremos (recorrer fila, columna o region)
    //r0 = direccion tablero
    //r1,r2,r3 //Se usan como registros auxiliares y como parametros de entrada-salida
    mov r4, #0 //int celdas_vacias = 0;
    mov r5, #0 //int i = 0;
	mov r6, #0 //int j;
	mov r7, #9 //Tamaño tablero

    B testBucleActualizarFilasALL
	bucleActualizarFilasALL:
		B testBucleActualizarColumnasALL
		bucleActualizarColumnasALL:
			//valor = celda_leer_valor(cuadricula[i][j]);
    		ldr r0, [sp] // cargamos en el registro 0 la dirección de inicio de la cuadricula
			mov r1, r5, LSL #5 //i * 32
			mov r2, r6, LSL #1 // j * 2
    		add r3, r1, r2   // DESPLAZAMIENTO
    		ldr r0, [r0,r3] //Cargamos el valor en r0 para llamar a la función "celda_leer_valor"
			bl celda_leer_valor
			STMFD sp!, {r0} //Guardamos en la pila el valor devuelto de la funcion "celda_leer_valor"
			cmp r0, #0 //if(valor == 0)
			beq celdaVacia //Si es igual saltamos a celdaVacia
 			ldrne r0, [sp] // cargamos en r0 la direccion de la cuadricula
			//movne r1, r5 // cargamos en r1 la variable i (Esta instruccion nos la podemos ahorrar)
			//movne r2, r6 // cargamos en r2 la variable j (Esta instruccion nos la podemos ahorrar)

				//----------------------------------------------------------------------------------------
				//recorrer fila descartando valor de listas candidatos
				//r0 = Direccion tablero
				//r1 = valor celda tablero
				//r2 = auxiliar desplazamiento columnas
				//r4,r5,r6,r7 no los usamos en esta parte del código
				//r8 = iterador
 				//r9,r10,r11 no los usamos en esta parte del código
				mov r8, #0
    			ldr r0, [sp, #4] //Cargamos en r0 la direccion inicial del tablero
    			//ldr r1, [sp, #8] //Cargamos en r1 la fila inicial (Podemos eliminar esta instruccion)
    			mov r2, r5, LSL #5 // Calculamos cual es la fila donde tenemos que propagar (Filas* 32)
    			add r0, r0, r2 //Sumamos a la direccion inicial la fila donde vamos a empezar a propagar
				B testBucleColumnasALL
				bucleColumnasALL:
					ldr r1, [sp] //Cargamos el valor de celda_leer_valor(cuadricula[fila][columna]) como segundo parametro de la funcion "celda_eliminar_candidato"
    				bl celda_eliminar_candidato
    				add r0, r0, #2 //Nos deplazamos por las columnas de la fila
					add r8, r8, #1 //columna_inicio++
				testBucleColumnasALL:
				cmp r8, r7
				bne bucleColumnasALL
				//------------------------------------------------------------------------------------
				// recorrer columna descartando valor de listas candidatos
				//r0 = Direccion tablero
				//r1 = valor celda tablero
				//r2 = auxiliar desplazamiento columnas
				//r4,r5,r6,r7 no los usamos en esta parte del código
				//r8 = iterador
 				//r9,r10,r11 no los usamos en esta parte del código
	 			mov r8, #0 //Reseteamos el iterador
	 			ldr r0, [sp,#4] //Cargamos en r0 la direccion inicial del tablero
	 			//ldr r2, [sp, #12] //Cargamos en r2 el número de columnas desde la pila (Podemos ahorrar esta instruccion)
	 			mov r2, r6, LSL #1 // Calculamos cual es la columna donde tenemos que propagar (columnas * 2)
     			add r0, r0, r2 //Sumamos a la direccion inicial la columna donde vamos a empezar a propagar
	 			B testBucleFilasALL
				bucleFilasALL:
					ldr r1, [sp] //Cargamos el valor de celda_leer_valor(cuadricula[fila][columna]) como segundo parametro de la funcion "celda_eliminar_candidato"
    				bl celda_eliminar_candidato
    				add r0, r0, #32 //Nos deplazamos por de la filas
					add r8, r8, #1 //columna_inicio++
				testBucleFilasALL:
				cmp r8, r7
				bne bucleFilasALL
				//-------------------------------------------------------------------------------------------------------------------------------------------------------------
				/* recorrer region descartando valor de listas candidatos */

				//r0 = Direccion tablero
				//r1 = valor celda tablero
				//r2,r3 = auxiliares
				//r4 = no los usamos en esta parte del código
				//r5 = Filas
				//r6 = Columnas
				//r7 = no los usamos en esta parte del código
				//r8 = Fila inicio
 				//r9 = Columna inicio
 				//r10 = Fila final
 				//r11 = Columna final

				//Se averigua en que cuadrante nos encontramos
 				//CALCULAMOS LA FILA INICIAL DEL CUADRANTE
    			//ldr r1, [sp, #8] //Cargamos en r1 la fila donde estamos
    			cmp r5, #3 //if(i < 3)
				movlt r8, #0 //less than

				cmp r5, #6   //if (i < 6)
				cmplt r5, #3 //&& (i >=3 )
				movge r8, #3

				cmp r5, #6 //if(i => 6)
				movge r8, #6

				//CALCULAMOS LA COLUMNA INICIAL DEL CUADRANTE
				//ldr r2, [sp, #12] //Cargamos en r2 la columna donde estamos
    			cmp r6, #3 //if(j < 3)
				movlt r9, #0 //less than

				cmp r6, #6   //if (j < 6)
				cmplt r6, #3 //&& (j >=3 )
				movge r9, #3

				cmp r6, #6 //if(j => 6)
				movge r9, #6

				add r10, r8, #3 //fila_final_region = fila_inicio + 3;
				add r11, r9, #3 //columna_final_region = columna_inicio + 3;

				STMFD sp!, {r9} //Guardamos en la pila la columna inicio para resetear
    			B testBucleFilasGrandeALL
				bucleFilasGrandeALL:
					B testBucleColumnasPequenioALL
					bucleColumnasPequenioALL:
					    ldr r0, [sp,#8] //Cargamos en r0 la direccion inicial del tablero
	    				ldr r1, [sp, #4] //Cargas en r1 el valor
    					mov r2, r8, LSL #5  //Multiplicamos por 32 el registro 7 (fila inicio cuadrante)
						mov r3, r9, LSL #1 //Multiplicamos por 2 el registro 8 (columna inicio cuadrante)
						add r3, r2, r3 //Sumamos la posición fila cuadrante + posicion columna cuadrante
						add r0, r0, r3 //Direccin inicio region = Direccion inicio + (filas* 32) + (columnas * 2)
						bl celda_eliminar_candidato
    					add r9, r9, #1 // columna_inicio ++
					testBucleColumnasPequenioALL:
					cmp r9, r11 //Compara columnaInicio con columnaFinal
					bne bucleColumnasPequenioALL
				add r8, r8, #1 //fila_inicio++
				ldr r9, [sp] // Cargamos de la pila columna inicio para resetearla
				testBucleFilasGrandeALL:
				cmp r8, r10 //Compara las filas con el final de la region
				bne bucleFilasGrandeALL

				LDMFD sp!, {r9} //Quitamos de la pila la columna inicio usada para resetear las columnas
				////////////////////////////////
				LDMFD sp!, {r0}//Quitamos de la pila el valor devuelto de la funcion "celda_leer_valor"

				finalPropagar:
			//++++++++++++++++++++++++++++++
			add r6, r6, #1 //j++
		testBucleActualizarColumnasALL:
		cmp r6, r7 //Comparamos columnas con el final de tablero
		bne bucleActualizarColumnasALL
		add r5, r5, #1 //i++
		mov r6, #0 //j = 0;
	testBucleActualizarFilasALL:
	cmp r5, r7 //Compara las filas con el final de tablero
	bne bucleActualizarFilasALL

    LDMFD sp!, {r0} //Quitamos el valor inicial de r0 guardado en la pila al inicio de la subrutina ya que tiene que devolver el número de celdas vacías
	mov r0, r4 //retornamos numero celdas vacias
	LDMFD sp!, {r1-r11,lr}
	mov pc, lr
celdaVacia:
	add r4, r4, #1 //celdas_vacias++
	LDMFD sp!, {r0} //Eliminamos el valor de la celda vacía de la pila
	b  finalPropagar
.end
#        END
