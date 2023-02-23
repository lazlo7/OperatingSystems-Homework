#include "../message.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

int shmid;
message_t* message;

void sigint_handler(int signum)
{
    printf("[Client] Received SIGINT, disconnecting from the server\n");
    message->type = MESSAGE_TYPE_DISCONNECT;

    if (shmdt(message) < 0) {
        printf("[Client error] Failed to detach shared memory segment with id %d\n", shmid);
        exit(1);
    }

    printf("[Client] Exit\n");
    exit(0);
}

int main()
{
    shmid = shmget(SHM_KEY, sizeof(message_t), 0);
    if (shmid < 0) {
        printf("[Client error] Failed to get shared memory segment with key %d\n", SHM_KEY);
        return 1;
    }

    message = shmat(shmid, NULL, 0);
    if (message == (void*)-1) {
        printf("[Client error] Failed to attach shared memory segment with id %d\n", shmid);
        return 1;
    }

    signal(SIGINT, sigint_handler);
    srand(time(NULL));
    printf("[Client] Started with shared memory segment id %d (ctrl+c to exit)\n", shmid);

    while (1) {
        const int random_number = rand() % 1000;

        sprintf(message->data, "%d", random_number);
        message->type = MESSAGE_TYPE_SEND;
        printf("[Client] Sent to the server: %d\n", random_number);

        sleep(3);
    }

    return 0;
}