.syntax unified 					// Sintaxis GCC
.cpu cortex-m4 						// CPU para el set de instrucciones del M4
.thumb 								// Set de instrucciones mixtas
.section .text						// En memoria de programa
.align 2							// Dirección par para alojar las instrucciones en memoria

// ---------- FIRST PROGRAM ----------
.global fst_function_asm 			// Función ASM esta variable a otros archivos
.type fst_function_asm, %function	// La etiqueta es una función

fst_function_asm: 					// Llego a esta función y vuelvo
	PUSH {R4}						// Opero sobre la pila. Variable "R4" a la pila. Almaceno en RAM
	MOV  R4,#1						// Modifico el registro. Guardo el valor 1 en la variable
	POP  {R4}						// Opero sobre la pila. Restauro el registro
	BX   LR							// Retorna. Dirección apuntada al registro

// ---------- SECOND PROGRAM ----------
.global snd_function_asm 			// Función ASM esta variable a otros archivos
.type snd_function_asm, %function	// La etiqueta es una función

snd_function_asm: 					// Llego a esta función y vuelvo
	ADD  R0,R0,1					// R0 = R0 + 1
	BX   LR							// Retorna. Dirección apuntada al registro

// ---------- THIRD PROGRAM ----------
.global rotate_right_asm 			// Función ASM esta variable a otros archivos
.type rotate_right_asm, %function	// La etiqueta es una función

rotate_right_asm:
	RORS R0,R0,R1
	BX   LR

// ---------- FOURTH PROGRAM ----------
.global rotate_left_asm 			// Función ASM esta variable a otros archivos
.type rotate_left_asm, %function	// La etiqueta es una función

rotate_left_asm:
    PUSH {R3, LR}
    // RSBS R2,R1,#32
   	// RORS R0,R0,R2
	// POP  {R2,PC}

	// Mejora: Usamos R3 como temporal para no usar PUSH/POP si no es necesario,
    // pero como usamos LR, hay que guardarlo. Cambiamos RSBS por una resta simple.

    MOVS R3,#32
	SUBS R3,R3,R1     				// R3 = 32 - R1 (el equivalente a un rotar a la izquierda)
	RORS R0,R0,R3
    POP  {R3,PC}       				// Carga LR directamente en PC para retornar más rápido


// ---------- FIFTH PROGRAM ----------
.global sum_vector_asm 				// Función ASM esta variable a otros archivos
.type sum_vector_asm, %function		// La etiqueta es una función

sum_vector_asm:
	// R0: Puntero a los datos
	// R1: Cantidad de datos
	// R2: Acumulador
	// R3: Variable temporal
	MOVS R2,#0						// Establezco el valor "0" en el registro R2

FST_RECURSIVE_LOOP:					// Etiquta más explita para llamar a la dirección LDR (deber ser la primer instrucción)
	// LDR	 R3,[R0]				// Establezco el contenido de R0 a R3. R3 = *R0
	// ADDS R2,R2,R3				// R2 = R2 + R3
	// ADDS R0,R0,#4				// Incremento el puntero en 4 (uint32_t x2). Serán 4 registros invrementados en lugar de 1.
	// SUBS R1,R1,#1				// Decremento de acuerdo a la longirud del vector
	// CMP  R1,#0					// Comparar registro R1 con "0" y actualizo NZCV (flags). Si el resultado de la operación es "0" romperá el lazo.

	// Mejora: Uso de Post-Indexed Addressing.
    // LDR R3, [R0], #4 carga el dato y aumenta el puntero en 4 en UN SOLO PASO.
    LDR  R3,[R0],#4   				// R3 = *R0, luego R0 = R0 + 4
    ADDS R2,R2,R3
    SUBS R1,R1,#1    				// SUBS ya actualiza el flag Z
    // Eliminamos CMP R1, #0 por ser redundante

	BNE  FST_RECURSIVE_LOOP			// Repite el lazo de acuerdo al resultado de CMP
	MOVS R0,R2						// Retorno por el registro R0
	BX   LR							// Retorna. Dirección apuntada al registro

// ---------- SIXTH PROGRAM ----------
.global memset_vector_asm 			// Función ASM esta variable a otros archivos
.type memset_vector_asm, %function	// La etiqueta es una función

memset_vector_asm:
	// R0: Puntero, R1: Dato, R2: Cantidad
    // Verificación de seguridad: si R2 es 0, la función fallaría.
    CBZ  R2,EXIT_MEMSET    // Compare and Branch if Zero

SND_RECURSIVE_LOOP:
	STRB R1,[R0],#1					// *R0 = R1, luego R0 = R0 + 1
	SUBS R2,R2,#1					// R2 = R2 - 1
	BNE  SND_RECURSIVE_LOOP			// Repite el lazo de acuerdo al resultado de CMP

EXIT_MEMSET:
    BX   LR

// ---------- SEVENTH PROGRAM ----------
.global xorshift32_asm 				// Función ASM esta variable a otros archivos
.type xorshift32_asm, %function		// La etiqueta es una función

xorshift32_asm:						// Marsaglia's function
    LDR  R1, [R0]          			// R1 = *estado (cargamos el valor actual)
    // x ^= x << 13;
    LSLS R2, R1, #13       			// R2 = x << 13
    EORS R1, R1, R2        			// x = x ^ R2
    // x ^= x >> 17;
    LSRS R2, R1, #17       			// R2 = x >> 17
    EORS R1, R1, R2       			// x = x ^ R2
    // x ^= x << 5;
    LSLS R2, R1, #5       			// R2 = x << 5
    EORS R1, R1, R2        			// x = x ^ R2
    STR  R1, [R0]          			// *estado = x (guardamos el nuevo valor)
    BX   LR                			// Retorno

// --- NOTAS GENERALES ---
// - La interfaz entre el assembler y C que vamos a utilizar va a ser por medio de funciones,
// el pasaje de datos será a través de los parámetros de la misma.
// - Para ARM la interfaz entre C t ASM est+a definida por el documento "ARM Architecture Procedura Call Standard (AAPCS)".
// - El lenguaje C no tiene operadores para rotar bits.
// - El uso del sufijo "S" (Set Flags) en las instrucciones (ADDS, SUBS, MOVS, EORS):
// Le indica al procesador que debe actualizar los flags del registro de estado APSR (Application Program Status Register)
// basándose en el resultado de esa operación.
// - ADDS / SUBS: Estas instrucciones realizan la suma o resta y actualizan los flags N (Negative), Z (Zero), C (Carry) y V (Overflow).
// En tu función sum_vector_asm, el uso de SUBS R1, R1, #1 es vital porque, al llegar el registro R1 a cero, el procesador activa el flag Z.
// Esto permite que la instrucción posterior BNE (Branch if Not Equal / Z=0) decida si debe repetir el lazo o terminar la ejecución.
// - MOVS: Se utiliza para mover un valor a un registro y verificar inmediatamente si ese valor es cero o negativo sin necesidad de una instrucción CMP adicional.
// - EORS / LSLS: En el algoritmo xorshift, se utilizan para asegurar que el código sea compatible con el set de instrucciones de 16 bits de los procesadores Cortex-M.
// Aunque en este algoritmo lineal no siempre evaluamos los flags, el uso de la "S" permite generar un binario más compacto y eficiente en términos de memoria de programa.

// --- INSTRUCCIONES ---
// LDR 				/**/ Interpretation: Load Register 					/**/ Action: Carga un valor de 32 bits desde la memoria a un registro [cite: 17, 40]
// STR 				/**/ Interpretation: Store Register 				/**/ Action: Almacena un valor de 32 bits desde un registro a la memoria [cite: 17, 40]
// MOV				/**/ Interpretation: Move 							/**/ Action: Copia un valor de un registro a otro o carga una constante inmediata
// MOVS 			/**/ Interpretation: Move and Set flags 			/**/ Action: Igual que MOV, pero actualiza los flags NZCV
// LSL / LSR 		/**/ Interpretation: Logical Shift Left / Right 	/**/ Action: Desplazamiento de bits (multiplicación/división por potencias de 2)
// ADD / ADC 		/**/ Interpretation: Add / Add with Carry 			/**/ Action: Suma simple o suma considerando el acarreo del flag C [cite: 9, 17]
// SUB / SBC 		/**/ Interpretation: Subtract / Subtract with Carry	/**/ Action: Resta simple o resta considerando el acarreo [cite: 9, 17]
// SUBS 			/**/ Interpretation: Subtract and Set flags 		/**/ Action: Resta y actualiza flags (vital para controlar bucles con BNE) [cite: 9, 17]
// MUL 				/**/ Interpretation: Multiply 						/**/ Action: Multiplicación de registros (el resultado son los 32 bits menos significativos) [cite: 9, 17]
// AND / ORR / EOR 	/**/ Interpretation: Bitwise AND, OR, Exclusive OR	/**/ Action: Operaciones lógicas bit a bit
// CMP 				/**/ Interpretation: Compare 						/**/ Action: Resta interna que no guarda el resultado, solo actualiza los flags para saltos condicionales
// B / BX 			/**/ Interpretation: Branch / Branch Exchange 		/**/ Action: Salto incondicional a una etiqueta o a una dirección en un registro
// BL / BLX 		/**/ Interpretation: Branch with Link 				/**/ Action: Salto a función guardando la dirección de retorno en el registro LR
// PUSH / POP 		/**/ Interpretation: Push / Pop Registers			/**/ Action: Guarda o recupera registros de la pila (SP) para preservar el contexto [cite: 12, 17]

// --- ARQUITECTURA ARM ---
// R0 		/**/ Interpretation: Primer parámetro de entrada y registro de retorno (32-bit) 				/**/ AAPCS: Scratch
// R1 		/**/ Interpretation: Segundo parámetro de entrada y parte alta del retorno (64-bit) 			/**/ AAPCS: Scratch
// R2-R3 	/**/ Interpretation: Tercer y cuarto parámetro de entrada / Uso temporal 						/**/ AAPCS: Scratch
// R4-R11 	/**/ Interpretation: Variables locales. Deben ser preservadas (Push/Pop) si se utilizan 		/**/ AAPCS: Callee-saved
// R12 (IP)	/**/ Interpretation: Intra-Procedure-call scratch register / Uso temporal 						/**/ AAPCS: Scratch
// R13 (SP)	/**/ Interpretation: Stack Pointer. Puntero a la cima de la pila. Siempre alineado a 8 bytes	/**/ AAPCS: Reserved
// R14 (LR)	/**/ Interpretation: Link Register. Almacena la dirección de retorno de funciones (BL/BLX) 		/**/ AAPCS: Scratch
// R15 (PC)	/**/ Interpretation: Program Counter. Dirección de la instrucción actual en ejecución 			/**/ AAPCS: Reserved
// S0-S15 	/**/ Interpretation: Parámetros de entrada de punto flotante y retorno (Single Precision) 		/**/ AAPCS: Scratch (VFP)
// S16-S31	/**/ Interpretation: Variables locales de punto flotante. Deben ser preservadas 				/**/ AAPCS: Callee-saved (VFP)
// FPSCR	/**/ Interpretation: Floating-Point Status and Control Register. Flags de la FPU 				/**/ AAPCS: Status
// APSR		/**/ Interpretation: Application Program Status Register. Contiene los flags NZCV				/**/ AAPCS: Status

// --- CONDICIONALES ---
// EQ 		/**/ Interpretation: Equal / Equals zero 					/**/ Status Flag State: Z set
// NE 		/**/ Interpretation: Not equal 								/**/ Status Flag State: Z clear
// CS / HS 	/**/ Interpretation: Carry set / Unsigned higher or same 	/**/ Status Flag State: C set
// CC / LO 	/**/ Interpretation: Carry clear / Unsigned lower 			/**/ Status Flag State: C clear
// MI 		/**/ Interpretation: Minus / Negative 						/**/ Status Flag State: N set
// PL 		/**/ Interpretation: Plus / Positive or zero 				/**/ Status Flag State: N clear
// VS 		/**/ Interpretation: Overflow 								/**/ Status Flag State: V set
// VC 		/**/ Interpretation: No overflow 							/**/ Status Flag State: V clear
// HI 		/**/ Interpretation: Unsigned higher 						/**/ Status Flag State: C set and Z clear
// LS 		/**/ Interpretation: Unsigned lower or same 				/**/ Status Flag State: C clear or Z set
// GE 		/**/ Interpretation: Signed greater than or equal 			/**/ Status Flag State: N equals V
// LT 		/**/ Interpretation: Signed less than 						/**/ Status Flag State: N is not equal to V
// GT 		/**/ Interpretation: Signed greater than 					/**/ Status Flag State: Z clear and N equals V
// LE 		/**/ Interpretation: Signed less than or equal 				/**/ Status Flag State: Z set or N is not equal to V
// AL 		/**/ Interpretation: Always (optional) 						/**/ Status Flag State: Any
