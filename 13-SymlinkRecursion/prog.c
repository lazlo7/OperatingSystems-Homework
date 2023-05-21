#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define OUT_DIR "./out/"
#define DUMMY_TEXT "Hello, World!\n"

void getFileName(int i, char* result) {
    sprintf(result, "%d", i);
}

int main(void) {
    int i = 1;
    char file_name_before[1024];
    char file_name_cur[1024];

    strcpy(file_name_before, OUT_DIR);
    getFileName(i, file_name_cur);
    strcat(file_name_before, file_name_cur);

    int fd = open(file_name_before, O_CREAT | O_WRONLY, 0666);
    if (fd == -1 && errno != EEXIST) {
        printf("[Error] Failed to create starting file: %s\n", strerror(errno));
        return 1;
    }

    if (write(fd, DUMMY_TEXT, sizeof(DUMMY_TEXT)) == -1) {
        printf("[Error] Failed to write dummy text to starting file: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    close(fd);
    strcpy(file_name_before, file_name_cur);
    printf("Created starting file\n");

    char buffer[1024];
    for (;;) {
        ++i;
        strcpy(file_name_cur, OUT_DIR);
        getFileName(i, buffer);
        strcat(file_name_cur, buffer);
        
        if (symlink(file_name_before, file_name_cur) == -1) {
            printf("[Error] Failed to create symlink: %s\n", strerror(errno));
            return 1;
        }

        printf("Created symlink: %s -> %s\n", file_name_cur, file_name_before);

        fd = open(file_name_cur, O_RDONLY);
        if (fd == -1) {
            if (errno == ELOOP) {
                printf("Got ELOOP, stopping\n");
                close(fd);
                break;
            }
            
            printf("[Error] Failed to open symlink: %s\n", strerror(errno));
            close(fd);
            return 1;
        }

        close(fd);

        getFileName(i, file_name_cur);
        strcpy(file_name_before, file_name_cur);
    }

    printf("Result: %d\n", i - 1);

    return 0;
}