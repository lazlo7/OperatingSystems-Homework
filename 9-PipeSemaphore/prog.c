#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#define MESSAGE_REPEAT_COUNT 10
#define SLEEP_DELAY 1

int main()
{
    int pipe_fds[2];

    if (pipe(pipe_fds) == -1) {
        printf("[Error] Failed to create pipe: %s\n", strerror(errno));
        return 1;
    }

    const key_t semaphore_read_key = ftok("prog.c", 0);
    if (semaphore_read_key == -1) {
        printf("[Error] Failed to create semaphore key: %s\n", strerror(errno));
        return 1;
    }

    const key_t semaphore_write_key = ftok("prog.c", 1);
    if (semaphore_write_key == -1) {
        printf("[Error] Failed to create semaphore key: %s\n", strerror(errno));
        return 1;
    }

    const int semaphore_read_id = semget(semaphore_read_key, 1, IPC_CREAT | 0666);
    if (semaphore_read_id == -1) {
        printf("[Error] Failed to create semaphore: %s\n", strerror(errno));
        return 1;
    }

    const int semaphore_write_id = semget(semaphore_write_key, 1, IPC_CREAT | 0666);
    if (semaphore_write_id == -1) {
        printf("[Error] Failed to create semaphore: %s\n", strerror(errno));
        return 1;
    }

    struct sembuf semaphore_read_buffer;
    semaphore_read_buffer.sem_num = 0;
    semaphore_read_buffer.sem_flg = 0;
    semaphore_read_buffer.sem_op = 0;

    struct sembuf semaphore_write_buffer;
    semaphore_write_buffer.sem_num = 0;
    semaphore_write_buffer.sem_flg = 0;
    semaphore_write_buffer.sem_op = 0;

    semctl(semaphore_write_id, 0, SETVAL, 1);
    semctl(semaphore_read_id, 0, SETVAL, 1);

    const int fork_result = fork();
    if (fork_result == -1) {
        printf("[Error] Failed to fork: %s\n", strerror(errno));
        return 1;
    }

    char buffer[1024];

    if (fork_result == 0) {
        // Child process.
        const char* child_message = "Child: Ping!";

        for (int i = 0; i < MESSAGE_REPEAT_COUNT; ++i) {
            write(pipe_fds[1], child_message, strlen(child_message) + 1);

            // Unblock the parent process after writing the message.
            semaphore_write_buffer.sem_op = -1;
            semop(semaphore_write_id, &semaphore_write_buffer, 1);

            // Block until the parent process reads the message.
            semaphore_read_buffer.sem_op = 0;
            semop(semaphore_read_id, &semaphore_read_buffer, 1);
            semctl(semaphore_read_id, 0, SETVAL, 1);

            // Read the response.
            read(pipe_fds[0], buffer, sizeof(buffer));
            printf("[Child] Received: '%s'\n", buffer);

            sleep(SLEEP_DELAY);
        }

    } else {
        // Parent process.
        const char* parent_message = "Parent: Pong!";

        for (int i = 0; i < MESSAGE_REPEAT_COUNT; ++i) {
            // Block until the child process writes the message.
            semaphore_write_buffer.sem_op = 0;
            semop(semaphore_write_id, &semaphore_write_buffer, 1);
            semctl(semaphore_write_id, 0, SETVAL, 1);

            // Read the message from the child and write a response.
            read(pipe_fds[0], buffer, sizeof(buffer));
            printf("[Parent] Received: '%s'\n", buffer);

            write(pipe_fds[1], parent_message, strlen(parent_message) + 1);

            // Unblock the child process.
            semaphore_read_buffer.sem_op = -1;
            semop(semaphore_read_id, &semaphore_read_buffer, 1);

            sleep(SLEEP_DELAY);
        }

        // Remove semaphores in parent process.
        // They will be removed automatically in child process.
        if (semctl(semaphore_read_id, semaphore_read_buffer.sem_num, IPC_RMID, 0) < 0) {
            printf("[Error] Failed to remove semaphore: %s\n", strerror(errno));
        }

        if (semctl(semaphore_write_id, semaphore_write_buffer.sem_num, IPC_RMID, 0) < 0) {
            printf("[Error] Failed to remove semaphore: %s\n", strerror(errno));
        }
    }

    // Close unnamed pipe both in parent and child processes.
    if (close(pipe_fds[0]) < 0) {
        printf("[Error] Failed to close pipe: %s\n", strerror(errno));
    }

    if (close(pipe_fds[1]) < 0) {
        printf("[Error] Failed to close pipe: %s\n", strerror(errno));
    }

    return 0;
}
