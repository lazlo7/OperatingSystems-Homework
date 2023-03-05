#include "message.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

const char* shared_memory_name = "shared-memory";
message_t* addr;
int shm;

void cleanup()
{
#ifdef CLIENT_REMOVES_MEMORY
    if (shm_unlink(shared_memory_name) < 0) {
        printf("[Client error] Failed to unlink shared memory '%s'\n", shared_memory_name);
        exit(1);
    }

    printf("[Client] Unlinked shared memory '%s'\n", shared_memory_name);
#endif

    if (close(shm) < 0) {
        printf("[Client error] Failed to close shared memory '%s' file descriptor\n", shared_memory_name);
        exit(1);
    }

    printf("[Client] Exit\n");
}

void sigint_handler(int signum)
{
    printf("[Client] Received SIGINT, notifying the server and exiting...\n");
    addr->event = EVENT_CLIENT_CLOSED;
    cleanup();
    exit(0);
}

int main()
{
    const int shared_memory_size = sizeof(message_t);

    if ((shm = shm_open(shared_memory_name, O_CREAT | O_RDWR, 0666)) < 0 && errno != EEXIST) {
        printf("[Client] Failed to open shared memory '%s'\n", shared_memory_name);
        return 1;
    }

    addr = mmap(0, shared_memory_size, PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);
    if (addr == (message_t*)-1) {
        printf("[Client] Failed to get pointer to the shared memory '%s'\n", shared_memory_name);
        return 1;
    }

    signal(SIGINT, sigint_handler);
    clock_t last_sent_time = clock() / CLOCKS_PER_SEC;
    srand(time(NULL));
    printf("[Client] Started client with shared memory '%s' (ctrl+c to exit)\n", shared_memory_name);

    addr->event = EVENT_NOP;
    while (1) {
        if (addr->event == EVENT_SERVER_CLOSED) {
            printf("[Client] Server closed, exiting...\n");
            break;
        }

        const clock_t current_time = clock() / CLOCKS_PER_SEC;
        if (current_time - last_sent_time < 3) {
            continue;
        }

        const int random_number = rand() % 1000;
        sprintf(addr->data, "%d", random_number);
        addr->event = EVENT_DATA;
        printf("[Client] Sent to the server: %s\n", addr->data);
        last_sent_time = current_time;
    }

    cleanup();
    return 0;
}