// Escribir la siguiente en función en assembler que pone en cero un vector:
// void zero(uint32_t *p, uint32_t len);
// Recibe un puntero y la longitud (en words) del vector a poner en cero.

.syntax unified 					// Sintaxis GCC
.cpu cortex-m4 						// CPU para el set de instrucciones del M4
.thumb 								// Set de instrucciones mixtas
.section .text						// En memoria de programa
.align 2							// Dirección par para alojar las instrucciones en memoria

// ---------- SECOND EXAMPLE ----------
.global sec_ex_zero_asm 			// Función ASM esta variable a otros archivos
.type sec_ex_zero_asm, %function	// La etiqueta es una función

sec_ex_zero_asm:
    CBZ  R1, EXIT           // Si len (R1) es 0, salta a EXIT (no hay elementos que poner en cero)
    CBZ  R0, EXIT           // Si el puntero (R0) es NULL, salta a EXIT (error)
    PUSH {R4}               // Guarda el registro que se va a usar en la pila
    MOV  R4, #0             // R4 = 0, valor a escribir en cada posición del vector
LOOP:
    STR  R4, [R0]           // Escribe 0 en la memoria apuntada por R0
    ADD  R0, #4             // Avanza al siguiente word (4 bytes)
    SUBS R1, #1             // Decrementa el contador de palabras (len). Actualizando los flags de estado, veo ZERO si len llega a 0
    BNE  LOOP               // Si no se han puesto en cero todas las palabras, repite
    POP  {R4}               // Restaura el registro usado desde la pila
EXIT:
    BX   LR
