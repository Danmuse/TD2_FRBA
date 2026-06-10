#ifndef INC_COLA_H_
#define INC_COLA_H_

#include <stdint.h>

typedef struct {
    uint8_t *buffer;    // Puntero al array de datos
    uint32_t head;      // Índice de escritura
    uint32_t tail;      // Índice de lectura
    uint32_t len;       // Tamaño máximo del buffer
    uint32_t count;     // Elementos actuales (opcional, pero facilita la vida)
} cola_t;

void cola_inicializar(cola_t *cola, uint8_t *buf, uint32_t len);
void cola_encolar(cola_t *cola, uint8_t val);
uint8_t cola_desencolar(cola_t *cola);
int cola_haydatos(cola_t *cola);

#endif /* INC_COLA_H_ */
