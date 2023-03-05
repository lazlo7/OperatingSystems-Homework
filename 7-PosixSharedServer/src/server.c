#include "message.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

const char* shared_memory_name = "shared-memory";
message_t* addr;
int shm;

void cleanup()
{
#ifdef SERVER_REMOVES_MEMORY
    if (shm_unlink(shared_memory_name) < 0) {
        printf("[Server error] Failed to unlink shared memory '%s'\n", shared_memory_name);
        exit(1);
    }

    printf("[Server] Unlinked shared memory '%s'\n", shared_memory_name);
#endif

    if (close(shm) < 0) {
        printf("[Server error] Failed to close shared memory '%s' file descriptor\n", shared_memory_name);
        exit(1);
    }

    printf("[Server] Exit\n");
}

void sigint_handler(int signum)
{
    printf("[Server] Received SIGINT, notifying the client and exiting...\n");
    addr->event = EVENT_SERVER_CLOSED;
    cleanup();
    exit(0);
}

int main()
{
    const int shared_memory_size = sizeof(message_t);

    if ((shm = shm_open(shared_memory_name, O_CREAT | O_RDWR, 0666)) < 0 && errno != EEXIST) {
        printf("[Server] Failed to open shared memory '%s'\n", shared_memory_name);
        return 1;
    }

    if (ftruncate(shm, shared_memory_size) < 0) {
        printf("[Server] Failed to size shared memory '%s'\n", shared_memory_name);
        return 1;
    }

    addr = mmap(0, shared_memory_size, PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);
    if (addr == (message_t*)-1) {
        printf("[Server] Failed to get pointer to the shared memory '%s'\n", shared_memory_name);
        return 1;
    }

    signal(SIGINT, sigint_handler);
    printf("[Server] Started server with shared memory '%s' (ctrl+c to exit)\n", shared_memory_name);

    addr->event = EVENT_NOP;
    while (1) {
        if (addr->event == EVENT_CLIENT_CLOSED) {
            printf("[Server] Client closed, exiting..\n");
            break;
        }

        if (addr->event == EVENT_DATA) {
            printf("[Server] Received: %s\n", addr->data);
            addr->event = EVENT_NOP;
        }
    }

    cleanup();
    return 0;
}