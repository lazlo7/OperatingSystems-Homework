#include "common.h"
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Returns the length of result_buffer.
// The bits in result_buffer are stored in big endian order.
void to_binary(int value, bool* result_buffer)
{
    for (int i = 0; i < sizeof(value) * 8; ++i) {
        result_buffer[sizeof(value) * 8 - i - 1] = value >> i & 1;
    }
}

// Has the current bit has been acknowledged.
bool bitAcknowledged;

void onAcknowledgementReceive(int signum)
{
    bitAcknowledged = true;
}

void sendBit(int receiver_pid, bool bit, int bit_number)
{
    bitAcknowledged = false;

    printf("[Sender] Sending %d (%d/%lu)\n", bit, bit_number, sizeof(int) * 8);
    if (kill(receiver_pid, bit ? SIGNAL_BIT_1 : SIGNAL_BIT_0) == -1) {
        printf("[Sender Error] Failed to send bit: %s\n", strerror(errno));
        exit(1);
    }

    while (!bitAcknowledged) { }
}

int main()
{
    if (signal(SIGNAL_BIT_1, onAcknowledgementReceive) == SIG_ERR) {
        printf("[Sender Error] Failed to register handler for SIGNAL_BIT_1: %s\n",
            strerror(errno));
        return 1;
    }

    const int pid = getpid();
    printf("[Sender] My PID is %d\n", pid);

    printf("[Sender] Enter receiver PID: ");
    int receiver_pid;
    scanf("%d", &receiver_pid);

    printf("[Sender] Enter an integer to send: ");
    int value;
    scanf("%d", &value);

    bool binary_value[sizeof(value) * 8];
    to_binary(value, binary_value);

    printf("[Sender] %d in binary is ", value);
    for (int i = 0; i < sizeof(value) * 8; ++i) {
        printf("%d", binary_value[i]);
    }
    printf("\n");

    for (int i = 0; i < sizeof(value) * 8; ++i) {
        sendBit(receiver_pid, binary_value[i], i + 1);
    }

    if (kill(receiver_pid, SIGINT) == -1) {
        printf("[Sender Error] Failed to send end of transmission: %s\n", strerror(errno));
        exit(1);
    }

    printf("[Sender] End of tranmission.\n");

    return 0;
}
