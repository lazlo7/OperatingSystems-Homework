#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

void printUsage() {
    printf("Использование: ./filewriter <output_file> input_file...\n");
}

int openFile(const char* file_path, int flags) {
    const int fd = open(file_path, flags);
    if (fd == -1) {
        printf("[ошибка] Не удалось открыть файл '%s'\n", file_path);
        exit(1);
    }
    return fd;
}

void dumpFile(const char* file_path, char* buffer, int buffer_size, int ouput_fd) {
    const int fd = openFile(file_path, O_RDONLY);

    // Цикличное чтение.
    ssize_t read_bytes = 0;
    do {
        read_bytes = read(fd, buffer, buffer_size);
        if (read_bytes == -1) {
            printf("[ошибка] Не удалось прочитать очередной кусок файла '%s'\n", file_path);
            exit(1);
        }
        write(ouput_fd, buffer, read_bytes);
    } while (read_bytes == buffer_size);

    close(fd);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("[ошибка] Пропущен обязательный аргумент <output_file>\n");
        printUsage();
        return 1;
    }

    // Если argc = 2, то нет смысла открывать <output_file> для записи,
    // просто выйдем.
    if (argc == 2) {
        return 0;
    }

    const int output_fd = openFile(argv[1], O_WRONLY | O_CREAT | O_TRUNC);
    const int buffer_size = 1024;
    char buffer[buffer_size];

    for (int i = 2; i < argc; ++i) {
        dumpFile(argv[i], buffer, buffer_size, output_fd);
    }

    close(output_fd);

    return 0;
}