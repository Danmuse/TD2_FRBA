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
//         }
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
    // R0: *datos, R1: len
    // R2: crc, R3: poly, R4: i, R5: bit, R6: temp
    CBZ  R0, EXIT                  	// Si datos es NULL, salta a EXIT
    CBZ  R1, EXIT                  	// Si len es cero, salta a EXIT

    LDR  R2, =0xFFFFFFFF          	// crc = 0xFFFFFFFF
    // MVN  R2, #0                  // Alternativa: crc = ~0
    LDR  R3, =0xEDB88320          	// poly = 0xEDB88320

    PUSH {R4-R6}                  	// Guarda los registros R4 y R5 en la pila
    MOV  R4, #0                   	// i = 0

FOR_LOOP:
    CMP  R4, R1                   	// Compara i con len. (i < len)?
    BHS  END_FOR                    // Si i >= len, terminamos el bucle

    LDRB R6, [R0, R4]             	// Carga el byte datos[i] en R6
    EOR  R2, R2, R6               	// crc ^= datos[i]
    MOV  R5, #0                   	// bit = 0

BIT_LOOP:
    CMP  R5, #8                   	// Compara bit con 8. (bit < 8)?
    BHS  NEXT_BYTE                  // Si bit >= 8, terminamos el bucle

    ANDS R6, R2, #1               	// Verifica si el bit menos significativo de crc es 1
    LSR  R2, R2, #1               	// crc >>= 1
    BEQ  NEXT_BIT                   // Si no es 1, salta a NEXT_BIT

    EOR  R2, R2, R3               	// crc = (crc >> 1) ^ poly

NEXT_BIT:
    ADD  R5, R5, #1               	// bit++
    B    BIT_LOOP                  	// Repite el bucle de bits

NEXT_BYTE:
    ADD  R4, R4, #1               	// i++
    B    FOR_LOOP                  	// Repite el bucle principal

END_FOR:
    MVN  R0, R2                     // R0 = ~crc (Resultado final en R0)
    // EOR  R2, R2, #0xFFFFFFFF     // crc ^= 0xFFFFFFFF
    // MOV  R0, R2                  // Mueve el resultado final a R0
    POP  {R4-R6}                    // Restauramos los 3 registros
    BX   LR							// Retorna. Dirección apuntada al registro

EXIT:
    MOV  R0, #0                     // Retorno 0 si len=0 o datos=NULL
	BX   LR							// Retorna. Dirección apuntada al registro
