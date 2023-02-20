#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv)
{
    char pipe_name[256] = "chat-pipe.fifo";

    if (argc >= 2) {
        strcpy(pipe_name, argv[1]);
    }

    if (mknod(pipe_name, S_IFIFO | 0666, 0) < 0 && errno != EEXIST) {
        printf("Can't create FIFO\n");
        return 1;
    }

    const int buffer_size = 64;
    char buffer[buffer_size];
    char ping_message[buffer_size];
    sprintf(ping_message, "Bip-bop from pid %d\n", getpid());

    int fd = open(pipe_name, O_WRONLY);
    if (fd < 0) {
        printf("Can't open FIFO for writing\n");
        return 1;
    }

    if (write(fd, ping_message, buffer_size) < 0) {
        printf("Can't write to the FIFO\n");
        return 1;
    }

    printf("[pid: %d] Send: %s", getpid(), ping_message);
    if (close(fd) < 0) {
        printf("Can't close the FIFO");
        return 1;
    }

    fd = open(pipe_name, O_RDONLY);
    if (fd < 0) {
        printf("Can't open FIFO for reading\n");
        return 1;
    }

    if (read(fd, buffer, buffer_size) < 0) {
        printf("Can't read from the FIFO\n");
        return 1;
    }

    printf("[pid: %d] Received: %s", getpid(), buffer);
    if (close(fd) < 0) {
        printf("Can't close the FIFO");
        return 1;
    }

    return 0;
}