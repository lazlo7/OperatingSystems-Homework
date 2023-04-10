#include "common.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int value = 0;
int sender_pid;

void onBitReceive(int signum)
{
    if (signum != SIGNAL_BIT_0 && signum != SIGNAL_BIT_1) {
        return;
    }

    bool bit = signum == SIGNAL_BIT_1;
    printf("[Receiver] Received %d\n", bit);
    value = (value << 1) | bit;

    // Send acknowledgement.
    if (kill(sender_pid, SIGNAL_BIT_1) == -1) {
        printf("[Receiver Error] Failed to send acknowledgement: %s\n", strerror(errno));
        exit(1);
    }
}

void sigint_handler(int signum)
{
    printf("[Receiver] End of transmission.\n");
    printf("[Receiver] Received integer: %d\n", value);
    exit(0);
}

int main()
{
    if (signal(SIGNAL_BIT_0, onBitReceive) == SIG_ERR) {
        printf("[Receiver Error] Failed to register handler for SIGNAL_BIT_0: %s\n",
            strerror(errno));
        return 1;
    }

    if (signal(SIGNAL_BIT_1, onBitReceive) == SIG_ERR) {
        printf("[Receiver Error] Failed to register handler for SIGNAL_BIT_1: %s\n",
            strerror(errno));
        return 1;
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        printf("[Receiver Error] Failed to register handler for SIGINT: %s\n",
            strerror(errno));
        return 1;
    }

    const int pid = getpid();
    printf("[Receiver] My PID is %d\n", pid);

    printf("[Receiver] Enter sender PID: ");
    scanf("%d", &sender_pid);

    printf("[Receiver] Waiting for a signal...\n");
    for (;;) { }

    return 0;
}
