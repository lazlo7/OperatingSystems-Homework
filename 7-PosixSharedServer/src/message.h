#pragma once

typedef enum {
    EVENT_NOP, // no operation
    EVENT_DATA, // data available
    EVENT_SERVER_CLOSED,
    EVENT_CLIENT_CLOSED
} EVENT_TYPE;

typedef struct {
    EVENT_TYPE event;
    char data[128];  
} message_t;
