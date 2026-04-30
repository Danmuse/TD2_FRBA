// Implementar la siguiente función en lenguaje assembler ARMv7-M:
// uint32_t get_amplitude(int32_t * signal, uint32_t len);
// La función retorna el valor de la amplitud pico a pico de la señal que se pasa como argumento.
// Por ejemplo, si el vector signal[] contiene los siguientes elementos:
// [28 421 763 983 1023 866 607 216 -199 -574 -855 -970 -933 -732 -392  13]
// El valor de retorno será 1993, ya que el valor máximo es 1023 y el mínimo -970.

.syntax unified 					// Sintaxis GCC
.cpu cortex-m4 						// CPU para el set de instrucciones del M4
.thumb 								// Set de instrucciones mixtas
.section .text						// En memoria de programa
.align 2							// Dirección par para alojar las instrucciones en memoria

// ---------- FIRST EXAM ----------
.global fst_exam_get_aplitude_asm 			// Función ASM esta variable a otros archivos
.type fst_exam_get_aplitude_asm, %function	// La etiqueta es una función

fst_exam_get_aplitude_asm:
    PUSH {R4}                  			// Guarda el valor de R4 en la pila
    MOV  R3, 0x80000000				// R3 = 0x80000000, valor mínimo posible para un entero con signo
    SUB  R2, R3, #1				// R2 = R3 - 1, valor máximo posible para un entero con signo
    CBZ  R1, EXIT                    // Si len es 0, salta a EXIT
    CBZ  R0, EXIT                    // Si signal es NULL, salta a EXIT
LOOP:
    LDR  R4, [R0]                 // Carga el valor actual de signal[i] en R4
    CMP  R4, R2                  // Compara el valor actual con el mínimo
    BGE  UPDATE_MAX                // Si R4 >= R2, actualiza el máximo
    MOV  R2, R4                  // Actualiza el mínimo con el valor actual
UPDATE_MAX:
    CMP  R4, R3                  // Compara el valor actual con el máximo
    BLE  UPDATE_MIN                // Si R4 <= R3, actualiza el mínimo
    MOV  R3, R4                  // Actualiza el máximo con el valor actual
UPDATE_MIN:
    ADD  R0, #4                  // Avanza al siguiente elemento del vector signal
    SUBS R1, #1                  // Decrementa len
    BNE  LOOP                    // Si len no es cero, repite el ciclo
EXIT:
    POP  {R4}                  			// Restaura el valor de R4 desde la pila
    SUB  R0, R3, R2				// R0 = R3 - R2, calcula la amplitud pico a pico
	BX   LR							// Retorna. Dirección apuntada al registro

