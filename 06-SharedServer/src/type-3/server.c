#include "../message.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

int shmid;
message_t* message;

void sigint_handler(int signum)
{
    printf("[Server] Received SIGINT, exiting\n");

    if (shmdt(message) < 0) {
        printf("[Server error] Failed to detach shared memory segment with id %d\n", shmid);
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        printf("[Server error] Failed to remove shared memory segment with key %d\n", SHM_KEY);
        exit(1);
    }

    printf("[Server] Removed shared memory segment with key %d\n", SHM_KEY);
    printf("[Server] Exit\n");

    exit(0);
}

int main()
{
    shmid = shmget(SHM_KEY, sizeof(message_t), SHM_PERMS | IPC_CREAT);
    if (shmid < 0) {
        printf("[Server error] Failed to create shared memory segment with key %d\n", SHM_KEY);
        return 1;
    }

    message = shmat(shmid, NULL, 0);
    if (message == (void*)-1) {
        printf("[Server error] Failed to attach shared memory segment with id %d\n", shmid);
        return 1;
    }

    signal(SIGINT, sigint_handler);
    message->type = MESSAGE_TYPE_NOP;
    printf("[Server] Started with shared memory segment id %d (ctrl+c to exit)\n", shmid);

    while (1) {
        if (message->type == MESSAGE_TYPE_DISCONNECT) {
            printf("[Server] Client disconnected\n");
            message->type = MESSAGE_TYPE_NOP;
        }

        if (message->type == MESSAGE_TYPE_SEND) {
            printf("[Server] Received: %s\n", message->data);
            message->type = MESSAGE_TYPE_NOP;
        }
    }

    return 0;
}