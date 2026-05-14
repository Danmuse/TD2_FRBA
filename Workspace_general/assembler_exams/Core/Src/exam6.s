// Implementar la siguiente función en lenguaje assembler ARMv7-M:

// uint16_t chksum(uint16_t * datos, uint32_t len);

// La función retorna el resultado de la suma de verificación del vector datos. La suma se calcula como sigue:
// Se coloca en cero un acumulador de 32 bits.
// Se suma de manera iterativa los datos sin signo de 16 bits en ese acumulador.
// Una vez terminada la suma iterativa se suman entre sí la parte alta (16 bits) y la parte baja (16 bits), ese resultado se invierte y se deja en 16 bits.
// Se sugiere el uso de las instrucciones LSR, MVN y AND.

// La implementación en C es la siguiente:

// uint16_t chksum(uint16_t * datos, uint32_t len)
// {
//     uint32_t chk = 0;
//     for (int i=0; i<len; i++)
//     {
//         chk += datos[i];
//     }
//     chk  = (chk >> 16) + (chk & 0xFFFF);
//     chk  = ~chk;
//     chk &= 0xFFFF;
//     return (uint16_t)chk;
// }

// Por ejemplo, si el vector datos[] contiene los siguientes elementos:
// [0x4500, 0x003C, 0x0A1C, 0x4000, 0xFF06, 0x0000, 0xA8B0, 0x0319, 0xA8B0, 0x036C]

// El valor de retorno será 0x19BA.

.syntax unified 					// Sintaxis GCC
.cpu cortex-m4 						// CPU para el set de instrucciones del M4
.thumb 								// Set de instrucciones mixtas
.section .text						// En memoria de programa
.align 2							// Dirección par para alojar las instrucciones en memoria

// ---------- SIXTH EXAM ----------
.global six_exam_checksum_asm 			// Función ASM esta variable a otros archivos
.type six_exam_checksum_asm, %function	// La etiqueta es una función

six_exam_checksum_asm:
	// TODO

EXIT:
	BX   LR							// Retorna. Dirección apuntada al registro

