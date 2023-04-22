#include "common.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 2

void printUsage()
{
    printf("Usage: ./server [<server_port>]\n");
    printf("By default, <server_port> = %d\n", DEFAULT_PORT);
}

// Returns socket fd of the accepted connection.
// On error, returns -1.
int acceptConnection(int sock, client_type_t client_type)
{
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    char const* client_name = client_type == CLIENT_TYPE_SENDER ? "sender" : "receiver";
    if ((client_sock = accept(sock, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
        printf("[Server Error] Failed to accept %s connection: %s\n", client_name, strerror(errno));
        return -1;
    }

    char client_type_id = client_type == CLIENT_TYPE_SENDER ? 'S' : 'R';
    // Send client it's type.
    if (send(client_sock, &client_type_id, 1, 0) == -1) {
        printf("[Server Error] Failed to send assigning byte to %s: %s\n", client_name, strerror(errno));
        return -1;
    }

    return client_sock;
}

int main(int argc, char** argv)
{
    if (argc > 2) {
        printUsage();
        printf("[Client Error] Too many arguments!\n");
        return 1;
    }

    int server_port = DEFAULT_PORT;
    if (argc == 2) {
        sscanf(argv[1], "%d", &server_port);
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

    int exit_code = 0;

    // Bind to the port.
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("[Server Error] Failed to bind to port %d: %s\n", server_port, strerror(errno));
        exit_code = 1;
        goto cleanup_sock;
    }

    // Listen for incoming connections.
    if (listen(sock, MAX_CLIENTS) == -1) {
        printf("[Server Error] Failed to listen on port %d: %s\n", server_port, strerror(errno));
        exit_code = 1;
        goto cleanup_sock;
    }

    printf("[Server] Started on port %d\n", server_port);

    // Accept sender connection.
    printf("[Server] Waiting for sender to connect...\n");
    int const sender_sock = acceptConnection(sock, CLIENT_TYPE_SENDER);
    if (sender_sock == -1) {
        exit_code = 1;
        goto cleanup_sock;
    }

    // Accept receiver connection.
    printf("[Server] Waiting for receiver to connect...\n");
    int const receiver_sock = acceptConnection(sock, CLIENT_TYPE_RECEIVER);
    if (receiver_sock == -1) {
        exit_code = 1;
        goto cleanup_sender_sock;
    } 

    // Send a signal to sender to start transmission.
    // We do that just by sending a byte.
    if (send(sender_sock, "\0", 1, 0) == -1) {
        printf("[Server Error] Failed to send a signal to sender: %s\n", strerror(errno));
        exit_code = 1;
        goto cleanup_receiver_sock;
    }
    printf("[Server] Sent a signal to sender to start transmission\n");

    printf("[Server] Waiting for a message from sender...\n");

    char buffer[1024];
    for (;;) {
        // Receive data from sender and send it to receiver.
        // If sender sends EOT_MESSAGE, break the loop.
        memset(buffer, 0, sizeof(buffer));
        if (recv(sender_sock, buffer, sizeof(buffer), 0) == -1) {
            printf("[Server Error] Failed to receive data from sender: %s\n", strerror(errno));
            exit_code = 1;
            goto cleanup_receiver_sock;
        }

        // Send data to receiver.
        if (send(receiver_sock, buffer, strlen(buffer), 0) == -1) {
            printf("[Server Error] Failed to send data to receiver: %s\n", strerror(errno));
            exit_code = 1;
            goto cleanup_receiver_sock;
        }

        printf("[Server] Received from sender: \'%s\' -> Sent it to receiver\n", buffer);

        if (strcmp(buffer, EOT_MESSAGE) == 0) {
            printf("[Server] Sender sent \'%s\' message, exiting...\n", EOT_MESSAGE);
            break;
        }
    }

cleanup_receiver_sock:
    close(receiver_sock);
cleanup_sender_sock:
    close(sender_sock);
cleanup_sock:
    close(sock);

    printf("[Server] Exit.\n");

    return exit_code;
}
