#include "cola.h"

void cola_inicializar(cola_t *cola, uint8_t *buf, uint32_t len) {
    cola->buffer = buf;
    cola->len = len;
    cola->head = 0;
    cola->tail = 0;
    cola->count = 0;
}

void cola_encolar(cola_t *cola, uint8_t val) {
    if (cola->count < cola->len) {
        cola->buffer[cola->head] = val;
        cola->head = (cola->head + 1) % cola->len;
        cola->count++;
    }
}

uint8_t cola_desencolar(cola_t *cola) {
    uint8_t val = 0;
    if (cola->count > 0) {
        val = cola->buffer[cola->tail];
        cola->tail = (cola->tail + 1) % cola->len;
        cola->count--;
    }
    return val;
}

int cola_haydatos(cola_t *cola) {
    return (cola->count > 0);
}
