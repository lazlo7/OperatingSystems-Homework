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
    printf("[Client] SIGINT Received, exit.\n");
    cleanup();
    exit(0);
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printUsage(argv[0]);
        printf("[Client Error] Missing required argument: <server_ip>\n");
        return 1;
    }

    char const* server_ip = argv[1];
    int client_port = DEFAULT_PORT;
    if (argc >= 3) {
        sscanf(argv[2], "%d", &client_port);
    }

    // Register SIGINT handler.
    if (signal(SIGINT, onInterruptReceived) == SIG_ERR) {
        printf("[Client Error] Failed to register SIGINT handler: %s.\n", strerror(errno));
        return 1;
    }

    // Create socket.
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        printf("[Client Error] Failed to create socket: %s.\n", strerror(errno));
        return 1;
    }

    // Prepare address.
    struct sockaddr_in sock_addr;
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_addr.sin_port = htons(client_port);

    // Bind to the port.
    if (bind(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
        printf("[Client Error] Failed to bind to port %d: %s.\n", client_port, strerror(errno));
        cleanup();
        return 1;
    }

    // Prepare multicast request.
    struct ip_mreq multicast_req;
    multicast_req.imr_multiaddr.s_addr = inet_addr(server_ip);
    multicast_req.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_req, sizeof(multicast_req)) == -1) {
        printf("[Client Error] Failed to set multicast type: %s\n", strerror(errno));
        cleanup();
        return 1;
    }

    printf("[Client] Started on port %d.\n", client_port);
    printf("[Client] Waiting for messages from server...\n");

    char buffer[1024];
    for (;;) {
        int const received_length = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, NULL, 0);
        if (received_length == -1) {
            printf("[Client] Failed to received message from server: %s\n", strerror(errno));
        }
        // Terminate buffer with null.
        buffer[received_length] = '\0';
        printf("> %s\n", buffer);
    }

    return 0;
}
