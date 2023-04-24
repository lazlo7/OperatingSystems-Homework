#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void printUsage()
{
    printf("Использование: ./filewriter <output_file> input_file...\n");
}

int openFile(const char* file_path, int flags, int mode)
{
    const int fd = open(file_path, flags, mode);
    if (fd == -1) {
        printf("[ошибка] Не удалось открыть файл '%s'\n", file_path);
        exit(1);
    }
    return fd;
}

void dumpFile(const char* file_path, char* buffer, int buffer_size, int output_fd)
{
    const int fd = openFile(file_path, O_RDONLY, 0666);

    // Цикличное чтение.
    ssize_t bytes_read = 0;
    do {
        bytes_read = read(fd, buffer, buffer_size);
        if (bytes_read == -1) {
            printf("[ошибка] Не удалось прочитать очередной кусок файла '%s'\n",
                file_path);
            exit(1);
        }
        write(output_fd, buffer, bytes_read);
    } while (bytes_read == buffer_size);

    // Разделяем содержимое файлов.
    write(output_fd, "\n", 1);

    close(fd);
}

// Возвращает флаги исполняемого файла (user, group, other).
// Если файл не исполняемый, то возвращается 0.
mode_t isExecutableFile(const char* file_path)
{
    struct stat file_stat;
    if (stat(file_path, &file_stat) == -1) {
        printf("[ошибка] Не удалось получить информацию о файле '%s'\n",
            file_path);
        exit(1);
    }
    return file_stat.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH);
}

int main(int argc, char** argv)
{
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

    // По умолчанию открываем файл с правами 0666, т. к. mode не будет применен, если файлы уже существует. 
    const int default_output_mode = 0666;
    const int output_fd = openFile(argv[1], O_WRONLY | O_CREAT | O_TRUNC, default_output_mode);
    const int buffer_size = 64;
    char buffer[buffer_size];

    mode_t output_execution_mode = 0;
    for (int i = 2; i < argc; ++i) {
        output_execution_mode |= isExecutableFile(argv[i]);
        dumpFile(argv[i], buffer, buffer_size, output_fd);
    }

    // Если хотя бы один из файлов был исполняемым, то передадим права исполнения.
    if (output_execution_mode != 0) {
        if (fchmod(output_fd, default_output_mode | output_execution_mode) == -1) {
            printf("[ошибка] Не удалось сделать файл '%s' исполняемым\n", argv[1]);
            exit(1);
        }
    }

    close(output_fd);

    return 0;
}