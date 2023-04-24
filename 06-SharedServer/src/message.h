#pragma once

typedef enum {
    MESSAGE_TYPE_NOP, // no operation
    MESSAGE_TYPE_SEND,
    MESSAGE_TYPE_DISCONNECT,
} MESSAGE_TYPE;

typedef struct {
    MESSAGE_TYPE type;
    char data[128];
} message_t;

const int SHM_KEY = 0x1234;
const int SHM_PERMS = 0666;
