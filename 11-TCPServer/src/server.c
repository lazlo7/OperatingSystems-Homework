#include "common.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 2
typedef enum {
    SERVER_SETTING_SENDER,
    SERVER_SETTING_RECEIVER,
    SERVER_TRANSMITTING
} server_state_t;

void printUsage()
{
    printf("Usage: ./server [<server_port>]\n");
    printf("By default, <server_port> = 7\n");
}

int main(int argc, char** argv)
{
    if (argc > 2) {
        printUsage();
        printf("[Client Error] Too many arguments!\n");
        return 1;
    }

    int server_port;
    if (argc == 2) {
        sscanf(argv[1], "%d", &server_port);
    } else {
        server_port = 7;
    }

    int const sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1) {
        printf("[Server Error] Failed to create socket: %s\n", strerror(errno));
        return 1;
    }

    // Constructing address structure.
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port);

    // Bind to the port.
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("[Server Error] Failed to bind to port %d: %s\n", server_port, strerror(errno));
        close(sock);
        return 1;
    }

    // Listen for incoming connections.
    if (listen(sock, MAX_CLIENTS) == -1) {
        printf("[Server Error] Failed to listen on port %d: %s\n", server_port, strerror(errno));
        close(sock);
        return 1;
    }

    server_state_t server_state = SERVER_SETTING_SENDER;
    in_addr_t sender_ip;
    in_addr_t receiver_ip;
    printf("[Server] Started on port %d, waiting for sender...\n", server_port);

    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int const client_sock = accept(sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            printf("[Server Error] Failed to accept incoming connection: %s\n", strerror(errno));
            close(sock);
            return 1;
        }

        switch (server_state) {
        case SERVER_SETTING_SENDER: {
            printf("[Server] Sender connected, waiting for receiver...\n");
            char const client_type = '0';
            if (send(client_sock, &client_type, 1, 0) != 1) {
                printf("[Server Error] Failed to send client type to sender: %s\n", strerror(errno));
                close(client_sock);
                close(sock);
                return 1;
            }
            sender_ip = client_addr.sin_addr.s_addr;
            server_state = SERVER_SETTING_RECEIVER;
            break;
        }
        case SERVER_SETTING_RECEIVER: {
            printf("[Server] Receiver connected, starting transmission...\n");
            char const client_type = '1';
            if (send(client_sock, &client_type, 1, 0) != 1) {
                printf("[Server Error] Failed to send client type to receiver: %s\n", strerror(errno));
                close(client_sock);
                close(sock);
                return 1;
            }
            receiver_ip = client_addr.sin_addr.s_addr;
            server_state = SERVER_TRANSMITTING;

            // Signal sender to start transmission.
            break;
        }
        case SERVER_TRANSMITTING: {
            
        }
        }

        close(client_sock);
    }

    close(sock);
    return 0;
}
