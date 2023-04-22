#include "common.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void printUsage()
{
    printf("Usage: ./client <server_ip> [<server_port>]\n");
    printf("By default, <server_port> = 7\n");
}

void emulateSender(int sock)
{
    // The server will send us a bit to indicate that we can start sending data.
    // Until then, sender should wait.
    printf("[Sender] Waiting for a receiver to connect...\n");
    char server_ready;
    if (recv(sock, &server_ready, 1, 0) != 1) {
        printf("[Sender Error] Failed to receive server ready bit: %s\n", strerror(errno));
        return;
    }

    // Read data from stdin and send it to the server.
    // Stop reading and notify server when 'The End' is typed.
    printf("[Sender] Receiver connected.\n");
    printf("[Sender] Type messages to send to the receiver. Type 'The End' to exit.\n");

    char buffer[1024];
    for (;;) {
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin);
        if (strcmp(buffer, "The End") == 0) {
            // Send 'The End' to the server.
            if (send(sock, buffer, strlen(buffer), 0) != strlen(buffer)) {
                printf("[Sender Error] Failed to send message: %s\n", strerror(errno));
                return;
            }
            break;
        }

        // Send the message to the server.
        if (send(sock, buffer, strlen(buffer), 0) != strlen(buffer)) {
            printf("[Sender Error] Failed to send message: %s\n", strerror(errno));
            return;
        }
    }
}

void emulateReceiver(int sock)
{
    // Receive data from the server and print it to stdout.
    // Stop receiving and notify server when 'The End' is received.
    // Note that, since receiver starts after the sender, we don't need to wait for a bit.
    printf("[Receiver] Waiting for a message from server...\n");

    char buffer[1024];
    for (;;) {
        int const bytes_received = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            printf("[Receiver Error] Failed to receive message: %s\n", strerror(errno));
            return;
        }

        // Print the message to stdout.
        printf("> %s\n", buffer);

        // Check if the message is 'The End'.
        if (strcmp(buffer, "The End") == 0) {
            printf("[Receiver] Received 'The End', exiting...\n");
            break;
        }
    }

    printf("[Receiver] Exit.\n");
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printUsage();
        printf("[Client Error] Missing required argument: <server_ip>!\n");
        return 1;
    }

    if (argc > 3) {
        printUsage();
        printf("[Client Error] Too many arguments!\n");
        return 1;
    }

    char const* server_ip = argv[1];
    int server_port;
    if (argc == 3) {
        sscanf(argv[2], "%d", &server_port);
    } else {
        server_port = 7;
    }

    int const sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1) {
        printf("[Client Error] Failed to create socket: %s\n", strerror(errno));
        return 1;
    }

    // Constructing address structure.
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    // Connect to the server.
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("[Client Error] Failed to connect to server: %s\n", strerror(errno));
        close(sock);
        return 1;
    }

    // Receive client-type bit.
    // '0' denotes a sender, '1' - a receiver.
    printf("[Client] Waiting for server to assign client type...\n");
    client_type_t client_type;
    if (recv(sock, &client_type, 1, 0) != 1) {
        printf("[Client Error] Failed to receive client type: %s\n", strerror(errno));
        close(sock);
        return 1;
    }

    if (client_type == CLIENT_TYPE_SENDER) {
        printf("Server assigned client type: sender\n");
        emulateSender(sock);
    } else {
        printf("Server assigned client type: receiver\n");
        emulateReceiver(sock);
    }

    close(sock);
    return 0;
}
