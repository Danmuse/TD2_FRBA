// Apellido y nombre: Di Módica Brogna, Daniel Fabrizio

// Implementar la siguiente función en lenguaje assembler ARMv7-M:
// Descripción.
// void signal_limiter(int32_t *p, uint32_t len, int32_t threshold);
// La función procesa un vector de muestras de 32 bits con signo y aplica una limitación de amplitud de
// tipo Hard-Knee sobre el vector original (in-place). El umbral de limitación se define mediante el
// parámetro threshold (valor positivo).

// Reglas de procesamiento:
// 1. Si el valor absoluto de la muestra es menor o igual al umbral (|p[i]| ≤ threshold), la muestra no debe modificarse.
// 2. Si el valor absoluto de la muestra supera el umbral (|p[i]| > threshold), la muestra debe
// reemplazarse por el valor del umbral, conservando el signo original de la muestra.
// - Si p[i] > threshold, entonces p[i] = threshold.
// - Si p[i] < -threshold, entonces p[i] = -threshold.
// 3. La función debe iterar para cada palabra de 32 bits del vector.
// 4. Sí len = 0, la función debe retornar inmediatamente sin acceder a la memoria.
// 5. Evite hacer más de una lectura de memoria por iteración.
// 6. Sí p es NULL, la función debe retornar inmediatamente sin acceder a la memoria.
// 7. ¿Es posible reemplazar la lógica de comparación y condicional de este ejercicio utilizando
// únicamente la instrucción de hardware SSAT rd, #N, rs (Signed Saturate)? Justifique su
// respuesta analizando la flexibilidad del parámetro threshold frente al parámetro #N de la instrucción.

// --------------------------------------------------------------------------------

// --- RESPUESTA ---
// NO, porque #N debe ser constante en tiempo de compilación, pero threshold es dinámico y puede variar en tiempo de ejecución.
// SSAT solo satura a potencias de 2: [-2^(N-1), 2^(N-1)-1], que es un caso contrario a este, ya que el valor de umbral "1000" no es una potencia de 2.

.syntax unified 					// Sintaxis GCC
.cpu cortex-m4 						// CPU para el set de instrucciones del M4
.thumb 								// Set de instrucciones mixtas
.section .text						// En memoria de programa
.align 2							// Dirección par para alojar las instrucciones en memoria

.global signal_limiter
.type signal_limiter, %function

signal_limiter:
    // R0: *p, R1: len, R2: threshold
    // R3: |p[i]|, R4: muestra actual p[i], R5: valor limitado
    PUSH {R4, R5}                   // Guarda R4 y R5 en la pila
    CBZ  R0, EXIT                   // Si p es NULL, salta a EXIT
    CBZ  R1, EXIT                   // Si len es cero, salta a EXIT

LOOP:
    LDR  R4, [R0], #4               // Carga el valor actual de p[i] en R4
    MOV  R3, R4                     // Copia p[i] a R3 para calcular el valor absoluto
    CMP  R3, #0                     // Compara el valor absoluto con cero
    BGE  CHECK_THRESHOLD            // Si p[i] >= 0, salta a CHECK_THRESHOLD
    RSBS R3, R3, #0                 // Si p[i] < 0, calcula el valor absoluto (R3 = -p[i])

CHECK_THRESHOLD:
    CMP  R3, R2                     // Compara el valor absoluto con el valor de umbral
    BLE  CONTINUE_LOOP              // Si |p[i]| <= threshold, continúa al siguiente ciclo
    // Si |p[i]| > threshold, trunca la muestra
    CMP  R4, #0                     // Compara la muestra original con cero para determinar el signo
    BGE  LIMIT_POSITIVE             // Si p[i] >= 0, salta a LIMIT_POSITIVE
    RSBS R5, R2, #0                 // Si p[i] < 0, calcula -threshold en R5
    B    STORE_LIMITED

LIMIT_POSITIVE:
    MOV  R5, R2                     // Si p[i] >= 0, copia threshold a R5

STORE_LIMITED:
    STR  R5, [R0, #-4]              // Almacena el valor limitado en p[i]

CONTINUE_LOOP:
    SUBS R1, R1, #1
    BNE  LOOP

EXIT:
    POP  {R4, R5}                   // Restaura R4 y R5 desde la pila
    BX   LR

