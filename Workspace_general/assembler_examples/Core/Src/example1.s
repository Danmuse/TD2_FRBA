// Escribir la siguiente en función en assembler que convierte un carácter a mayúsculas:
// int32_t my_toupper(int32_t c)
// Si c es un carácter en minúscula la función retorna su mayúscula correspondiente.
// Si c es un carácter en mayúscula o un carácter no alfabético retorna ese mismo carácter.

.syntax unified 					// Sintaxis GCC
.cpu cortex-m4 						// CPU para el set de instrucciones del M4
.thumb 								// Set de instrucciones mixtas
.section .text						// En memoria de programa
.align 2							// Dirección par para alojar las instrucciones en memoria

// ---------- FIRST EXAMPLE ----------
.global fst_ex_toupper_asm 			// Función ASM esta variable a otros archivos
.type fst_ex_toupper_asm, %function	// La etiqueta es una función

fst_ex_toupper_asm:
    // --- Equivalent implementation ---
    // MOVS R1, 96      // R1 = 96, el valor a restar para convertir a mayúscula
    // MOVS R2, 123     // R2 = 123, el límite superior para caracteres minúsculos
    // CMP  R0, R1      // Compara el carácter en R0 con R1 (96)
    // BLS  EXIT        // Si R0 <= R1, salta a EXIT (no es minúscula)
    // CMP  R0, R2      // Compara el carácter en R0 con R2 (123)
    // BHS  EXIT        // Si R0 >= R2, salta a EXIT (no es minúscula)

    MOVS R1, #'a'    // R1 = 'a', el valor a restar para convertir a mayúscula
    MOVS R2, #'z'    // R2 = 'z', el límite superior para caracteres minúsculos
    CMP  R0, R1      // Compara el carácter en R0 con R1 (97)
    BLO  EXIT        // Si R0 < R1, salta a EXIT (no es minúscula)
    CMP  R0, R2      // Compara el carácter en R0 con R2 (122)
    BHI  EXIT        // Si R0 > R2, salta a EXIT (no es minúscula)

    SUBS R0, 32      // R0 = R0 - 32, convierte a mayúscula
EXIT:
	BX   LR

// ---------- FIRST EXAMPLE IMPROVED ----------
.global fst_ex_toupper_string_asm 			// Función ASM esta variable a otros archivos
.type fst_ex_toupper_string_asm, %function	// La etiqueta es una función

fst_ex_toupper_string_asm:
    MOVS R1, #'a'    // R1 = 'a', el valor a restar para convertir a mayúscula
    MOVS R2, #'z'    // R2 = 'z', el límite superior para caracteres minúsculos

LOOP:
    LDRB R3, [R0]    // Carga el byte del carácter en R3
    CMP  R3, #0      // Compara el carácter con el terminador de cadena (0)
    BEQ  EXIT        // Si es el terminador de cadena, salta a EXIT

    CMP  R3, R1      // Compara el carácter en R0 con R1 (97)
    BLO  NEXT        // Si R0 < R1, salta a la siguiente iteración (no es minúscula)
    CMP  R3, R2      // Compara el carácter en R0 con R2 (122)
    BHI  NEXT        // Si R0 > R2, salta a la siguiente iteración (no es minúscula)
    SUBS R3, #32     // R0 = R0 - 32, convierte a mayúscula
    STRB R3, [R0]    // Almacena el carácter convertido de vuelta en la cadena
NEXT:
    ADD R0, #1       // Avanza al siguiente carácter en la cadena (sin actualizar los Status flags)
    B   LOOP         // Repite el proceso para el siguiente carácter

