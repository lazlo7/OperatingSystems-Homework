#include "../common.h"
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int sock;

void printUsage(char const* cmd)
{
    printf("Usage: %s <server_ip> [<server_port>]\n", cmd);
    printf("By default, <server_port> = %d\n", DEFAULT_PORT);
}

void cleanup()
{
    close(sock);
}

void onInterruptReceived(int signum)
{
    printf("\n");
    printf("[Server] SIGINT Received, exit.\n");
    cleanup();
    exit(0);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printUsage(argv[0]);
        printf("[Server Error] Missing required argument: <server_ip>.\n");
        return 1;
    }

    char const* server_ip = argv[1];
    int server_port = DEFAULT_PORT;
    if (argc >= 3) {
        sscanf(argv[2], "%d", &server_port);
    }

    // Register SIGINT handler.
    if (signal(SIGINT, onInterruptReceived) == SIG_ERR) {
        printf("[Server Error] Failed to register SIGINT handler: %s.\n", strerror(errno));
        return 1;
    }

    // Create socket.
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        printf("[Server Error] Failed to create socket: %s.\n", strerror(errno));
        return 1;
    }

    // Set broadcast option.
    int broadcast_perm = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_perm, sizeof(broadcast_perm)) == -1) {
        printf("[Server Error] Failed to set broadcast type: %s.\n", strerror(errno));
        cleanup();
        return 1;
    }

    // Prepare address.
    struct sockaddr_in sock_addr;
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(server_ip);
    sock_addr.sin_port = htons(server_port);

    printf("[Server] Started on %s:%d.\n", server_ip, server_port);
    printf("[Server] Enter messages to send to clients. Press ctrl+c to quit.\n");

    char buffer[1024];
    for (;;) {
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin);
        // Remove trailing newline.
        buffer[strlen(buffer) - 1] = '\0';

        if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
            printf("[Server Error] Failed to send message to clients: %s\n", strerror(errno));
        }
    }

    return 0;
}
