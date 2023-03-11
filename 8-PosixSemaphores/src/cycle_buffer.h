#pragma once

#define CB_BUFFER_SIZE 10

// Simple FIFO circular buffer with limited size. 
typedef struct {
    int buffer[CB_BUFFER_SIZE];
    int head;
    int tail;
} CycleBuffer;

void cb_init(CycleBuffer *cb);
void cb_offer(CycleBuffer *cb, int i);
int cb_peek(CycleBuffer *cb); 
int cb_remove(CycleBuffer *cb);
