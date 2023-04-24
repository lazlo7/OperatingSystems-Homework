#include "cycle_buffer.h"

void cb_init(CycleBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
}

void cb_offer(CycleBuffer *cb, int i) {
    cb->buffer[cb->tail++] = i;
    cb->tail %= CB_BUFFER_SIZE;
}

int cb_peek(CycleBuffer *cb) {
    return cb->buffer[cb->head];
}

int cb_remove(CycleBuffer *cb) {
    const int i = cb->buffer[cb->head++];
    cb->head %= CB_BUFFER_SIZE;
    return i;
}
