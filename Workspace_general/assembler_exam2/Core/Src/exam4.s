// Implementar la siguiente función en lenguaje assembler ARMv7-M:

// uint32_t one_at_a_time_hash(uint8_t * datos, uint32_t len);

// La función retorna el resultado de la función hash de Bob Jenkins “one at a time”
// en 32 bits con la implementación en lenguaje C que se muestra a continuación

// uint32_t one_at_a_time_hash(uint8_t* datos, uint32_t len)
// {
//   uint32_t i;
//   uint32_t hash = 0;
//   for(i=0; i<len; i++)
//   {
//      hash += datos[i];
//      hash += hash << 10;
//      hash ^= hash >> 6;
//   }
//   hash += hash << 3;
//   hash ^= hash >> 11;
//   hash += hash << 15;
//   return hash;
// }

// Tenga las siguientes consideraciones:
// En caso de que la función reciba len = 0 retorna cero sin leer memoria.
// Tenga presente que le pueden ser de utilidad las instrucciones LSL, LSR y EOR para desplazamientos a izquierda,
// derecha y or exclusiva respectivamente.
// El llamado a función one_at_a_time_hash(“a”,1) genera una salida de 0xca2e9442
// El llamado a función one_at_a_time_hash(“Hola Mundo!”,11) genera una salida de 0x6bd97be8

.syntax unified 					// Sintaxis GCC
.cpu cortex-m4 						// CPU para el set de instrucciones del M4
.thumb 								// Set de instrucciones mixtas
.section .text						// En memoria de programa
.align 2							// Dirección par para alojar las instrucciones en memoria

// ---------- FOURTH EXAM ----------
.global fth_exam_one_at_a_time_hash_asm 			// Función ASM esta variable a otros archivos
.type fth_exam_one_at_a_time_hash_asm, %function	// La etiqueta es una función

fth_exam_one_at_a_time_hash_asm:
	// R0: puntero a uint8_t
	// R1: len
	// R2: hash
	// R3: tmp
	MOV  R2, #0
	CBZ  R0, EXIT
	CBZ  R1, EXIT

FOR_LOOP:
    LDRB R3, [R0], 1         			// Carga el valor actual de datos[i] en R3 y avanza el puntero
    ADD  R2, R2, R3         			// hash += datos[i]
    LSL  R3, R2, #10        			// R3 = hash << 10
    ADD  R2, R2, R3         			// hash += hash << 10
    LSR  R3, R2, #6         			// R3 = hash >> 6
    EOR  R2, R2, R3         			// hash ^= hash >> 6
    SUBS R1, #1                      // Decrementa len
    BNE  FOR_LOOP                   // Si len no es cero, repite el ciclo
    // x ^= x << 13;
    LSL  R3, R2, #3       			// R3 = x << 3
    ADD  R2, R2, R3         		// x += x << 3
    LSR  R3, R2, #11       			// R3 = x >> 11
    EOR  R2, R2, R3        			// x = x ^ R2
    LSL  R3, R2, #15       			// R3 = x << 15
    ADD  R2, R2, R3         		// x += x << 15

EXIT:
    MOV  R0, R2         			// Retorna el valor de hash en R0
	BX   LR
