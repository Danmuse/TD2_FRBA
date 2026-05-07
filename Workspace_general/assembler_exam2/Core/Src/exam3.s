// Un filtro IIR muy utilizado (polo simple digital) tiene la siguiente ecuación en recurrencia de su salida:
// y[n] = αx[n] + (1-α)y[n-1] con 0 < α < 1.
// Se desea implementar una versión en punto fijo con la siguiente expresión:
// y[n] = y[n-1] + ((x[n]-y[n-1])>>k)
// que se puede implementar en C como:

// void polo_simple(int32_t *x, int32_t *y, uint32_t len, uint32_t k)
// {
//     uint32_t i;
//     int32_t yp = 0;
//     for (i=0; i<len; i++)
//     {
//         yp = yp + ((x[i]-yp)>>k);
//         y[i] = yp;
//     }
// }

// Implementar la misma función en lenguaje assembler ARMv7-M teniendo en cuenta que el parámetro len puede ser cero.
// Se recomienda utilizar la instrucción ASR, escriba en un comentario el por qué de esta recomendación frente a LSR.

// Para los siguientes parámetros, la salida será la línea comentada:

// int32_t y[16];
// int32_t x[16] = {100, 100, 100, 100, 100, 100, 100, 100,
//                  100, 100, 100, 100, 100, 100, 100, 100};
// uint32_t N = sizeof(x)/sizeof(x[0]);
// polo_simple(x, y, N, 2);
// y = {25, 43, 57, 67, 75, 81, 85, 88, 91, 93, 94, 95, 96, 97, 97, 97}

.syntax unified 					// Sintaxis GCC
.cpu cortex-m4 						// CPU para el set de instrucciones del M4
.thumb 								// Set de instrucciones mixtas
.section .text						// En memoria de programa
.align 2							// Dirección par para alojar las instrucciones en memoria

// ---------- THIRD EXAM ----------
.global trd_exam_simple_pole_asm 			// Función ASM esta variable a otros archivos
.type trd_exam_simple_pole_asm, %function	// La etiqueta es una función

trd_exam_simple_pole_asm:
	// R2: len
	// R3: k
	// R4: x[i]
	// R5: yp
	// R6: (x[i] - yp) >> k

    PUSH {R4-R7}
	CBZ  R0, EXIT
	CBZ  R1, EXIT
	CBZ  R2, EXIT
    MOV  R5, #0
FOR_LOOP:
    LDR  R4, [R0], #4                 // Carga el valor actual de x[i] en R4
    SUB  R6, R4, R5                  // R6 = x[i] - yp
    ASR  R6, R6, R3                  // R6 = (x[i] - yp) >> k, se recomienda ASR porque son int32_t con signo, mientras que LSR se usaría para uint32_t sin signo
    ADD  R5, R5, R6                  // yp = yp + ((x[i] - yp) >> k)
    STR  R5, [R1], #4                 // y[i] = yp
    SUBS R2, #1                      // Decrementa len
    BNE  FOR_LOOP                    // Si len no es cero, repite el ciclo
EXIT:
    POP  {R4-R7}
	BX   LR
/*
    PUSH {R4-R7, LR}
    // if(len == 0) return;
    CMP     R2, #0
    BEQ     EXIT
    // i = 0
    MOVS    R4, #0
    // yp = 0
    MOVS    R5, #0

FOR_LOOP:
    // if(i >= len) salir
    CMP     R4, R2
    BCS     EXIT
    // cargar x[i]
    // offset = i * 4
    LSLS    R6, R4, #2
    LDR     R7, [R0, R6]
    // R7 = x[i] - yp
    SUBS    R7, R7, R5
    // (x[i] - yp) >> k
    // usar ASR porque son int32_t con signo
    ASRS    R7, R7, R3
    // yp = yp + ...
    ADDS    R5, R5, R7
    // y[i] = yp
    STR     R5, [R1, R6]
    // i++
    ADDS    R4, R4, #1
    B       FOR_LOOP

EXIT:
    POP     {R4-R7, LR}
	BX   LR							// Retorna. Dirección apuntada al registro
*/
