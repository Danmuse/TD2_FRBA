// Implementar la siguiente función en lenguaje assembler ARMv7-M:
// uint32_t crc32(uint8_t * datos, uint32_t len);
// La función retorna el CRC32 con la implementación del IEEE 802.3 con el polinomio 0xEDB88320.
// La implementación en lenguaje C se muestra a continuación

// uint32_t crc32(uint8_t* datos, uint32_t len)
// {
//    uint32_t i;
//    uint32_t crc = 0xFFFFFFFF;
//    uint32_t poly = 0xEDB88320;
//    uint8_t bit;
//    for (i = 0; i < len; i++)
//    {
//         crc ^= datos[i];
//         for (bit = 0; bit < 8; bit++)
//         {
//             if (crc & 1)
//                 crc = (crc >> 1) ^ poly;
//             else
//                 crc >>= 1;
//        }
//     }
//     return crc ^ 0xFFFFFFFF;
// }

// Tenga las siguientes consideraciones:
// En caso de que la función reciba len = 0 retorna cero sin leer memoria.
// Tenga presente que le pueden ser de utilidad las instrucciones LSR y EOR para desplazamientos a derecha y or exclusiva respectivamente.
// Para cargar constantes de más de 8 bits se puede usar la pseudoinstrucción:
// LDR rd, =constante
// El llamado a función crc32(“a”,1) genera una salida de 0xe8b7be43
// El llamado a función crc32(“Hola Mundo!”,11) genera una salida de 0xf6940f45

.syntax unified 					// Sintaxis GCC
.cpu cortex-m4 						// CPU para el set de instrucciones del M4
.thumb 								// Set de instrucciones mixtas
.section .text						// En memoria de programa
.align 2							// Dirección par para alojar las instrucciones en memoria

// ---------- SECOND EXAM ----------
.global snd_exam_crc32_asm 			// Función ASM esta variable a otros archivos
.type snd_exam_crc32_asm, %function	// La etiqueta es una función

snd_exam_crc32_asm:
    PUSH {R4}                  			// Guarda el valor de R4 en la pila

EXIT:
    POP  {R4}                  			// Restaura el valor de R4 desde la pila
	BX   LR							// Retorna. Dirección apuntada al registro
/*
    PUSH {R4-R7, LR}

    // if(len == 0) return 0;
    CMP     R1, #0
    BEQ     crc_zero

    // crc = 0xFFFFFFFF
    LDR     R2, =0xFFFFFFFF

    // poly = 0xEDB88320
    LDR     R3, =0xEDB88320

    // i = 0
    MOVS    R4, #0

outer_loop:

    // if(i >= len) salir
    CMP     R4, R1
    BCS     finish

    // cargar byte: datos[i]
    LDRB    R5, [R0, R4]

    // crc ^= datos[i]
    EORS    R2, R2, R5

    // bit = 0
    MOVS    R6, #0

inner_loop:

    // if(bit >= 8) salir
    CMP     R6, #8
    BCS     next_byte

    // if(crc & 1)
    TST     R2, #1
    BEQ     crc_shift

crc_poly:

    // crc = (crc >> 1) ^ poly
    LSRS    R2, R2, #1
    EORS    R2, R2, R3
    B       bit_done

crc_shift:

    // crc >>= 1
    LSRS    R2, R2, #1

bit_done:

    ADDS    R6, R6, #1
    B       inner_loop

next_byte:

    ADDS    R4, R4, #1
    B       outer_loop

finish:

    // return crc ^ 0xFFFFFFFF
    LDR     R3, =0xFFFFFFFF
    EORS    R0, R2, R3

    POP     {R4-R7, LR}
    BX      LR

crc_zero:

    MOVS    R0, #0
    POP     {R4-R7, LR}
    BX      LR
*/
