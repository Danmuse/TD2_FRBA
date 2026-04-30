// Escribir la siguiente en función en assembler que verifica la salud de la memoria RAM del microcontrolador:
// int32_t memory_check(uint32_t *p, uint32_t len)
// Recibe un puntero y la longitud (en words) del área a verificar. Esta función verifica cada palabra de 32-bits de la siguiente manera:
// Escribe 0xAAAAAAAA en la posición de memoria apuntada, luego la lee y verifica que el valor leído sea el mismo.
// Escribe 0x55555555 en la posición de memoria apuntada, luego la lee y verifica que el valor leído sea el mismo.
// Itera para cada palabra del vector.
// Si encuentra que algún valor leído es diferente al escrito o si len = 0 la función devuelve -1. Si no encuentra problemas, devuelve 0.

.syntax unified 					// Sintaxis GCC
.cpu cortex-m4 						// CPU para el set de instrucciones del M4
.thumb 								// Set de instrucciones mixtas
.section .text						// En memoria de programa
.align 2							// Dirección par para alojar las instrucciones en memoria

// ---------- THIRD EXAMPLE ----------
.global trd_ex_memory_check_asm 			// Función ASM esta variable a otros archivos
.type trd_ex_memory_check_asm, %function	// La etiqueta es una función

trd_ex_memory_check_asm:
    MVN  R2, #0             // R2 = -1 (bitwise NOT de 0 = -1), valor de retorno en caso de error
    CBZ  R1, EXIT           // Si len (R1) es 0, salta a EXIT (error)
    CBZ  R0, EXIT           // Si el puntero (R0) es NULL, salta a EXIT (error)
    PUSH {R4-R5}            // Guarda los registros que se van a usar en la pila
	// --- Equivalent implementation ---
/*
LOOP:
    MOV  R4, #0xAAAAAAAA    // R4 = 0xAAAAAAAA, primer patrón de prueba
    STR  R4, [R0]           // Escribe el primer patrón en la memoria apuntada por R0
    LDR  R5, [R0]           // Lee el valor de la memoria apuntada por R0 en R5
    CMP  R5, R4             // Compara el valor leído con el patrón escrito
    BNE  END_ERROR          // Si no son iguales, salta a END_ERROR (error)
    MOV  R4, #0x55555555    // R4 = 0x55555555, primer patrón de prueba
    STR  R4, [R0]           // Escribe el primer patrón en la memoria apuntada por R0
    LDR  R5, [R0]           // Lee el valor de la memoria apuntada por R0 en R5
    CMP  R5, R4             // Compara el valor leído con el patrón escrito
    BNE  END_ERROR          // Si no son iguales, salta a END_ERROR (error)
    ADD  R0, #4             // Avanza al siguiente word (4 bytes)
    SUBS R1, #1             // Decrementa el contador de palabras (len). Actualizando los flags de estado, veo ZERO si len llega a 0
    BNE  LOOP               // Si no se han verificado todas las palabras, repite
END_ERROR:
    POP  {R4-R5}            // Restaura los registros usados desde la pila
    CMP  R1, #0             // Compara el contador de palabras con 0
    BNE  EXIT               // Si no se han verificado todas las palabras, salta a EXIT (error)
    MOV  R2, #0             // R2 = 0
EXIT:
    MOV  R0, R2             // R0 = R2, prepara el valor de retorno
	BX   LR
*/
LOOP:
    MOVW R4, #0xAAAA        // Cargar 16 bits bajos de 0xAAAAAAAA
    MOVT R4, #0xAAAA        // Cargar 16 bits altos de 0xAAAAAAAA
    STR  R4, [R0]           // Escribe el primer patrón en la memoria apuntada por R0
    LDR  R5, [R0]           // Lee el valor de la memoria apuntada por R0 en R5
    CMP  R5, R4             // Compara el valor leído con el patrón escrito
    BNE  ERROR_FOUND        // Si no son iguales, salta a ERROR_FOUND
    MOVW R4, #0x5555        // Cargar 16 bits bajos de 0x55555555
    MOVT R4, #0x5555        // Cargar 16 bits altos de 0x55555555
    STR  R4, [R0]           // Escribe el segundo patrón en la memoria apuntada por R0
    LDR  R5, [R0]           // Lee el valor de la memoria apuntada por R0 en R5
    CMP  R5, R4             // Compara el valor leído con el patrón escrito
    BNE  ERROR_FOUND        // Si no son iguales, salta a ERROR_FOUND
    ADD  R0, #4             // Avanza al siguiente word (4 bytes)
    SUBS R1, #1             // Decrementa el contador de palabras (len). Actualizando los flags de estado, veo ZERO si len llega a 0
    BNE  LOOP               // Si no se han verificado todas las palabras, repite
    MOV  R2, #0             // R2 = 0 (éxito, todos los tests pasaron)
    POP  {R4-R5}            // Restaura los registros usados desde la pila
    B    EXIT               // Salta a EXIT
ERROR_FOUND:
    POP  {R4-R5}            // Restaura los registros usados desde la pila
EXIT:
    MOV  R0, R2             // R0 = R2, prepara el valor de retorno
	BX   LR
