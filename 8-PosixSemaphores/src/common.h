#pragma once

// Заголовочный файл, содержащий общие данные для писателей и читателей
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "cycle_buffer.h"

#define SEM_BUF_SIZE 4

// Структура для хранения в разделяемой памяти буфера и необходимых данных
// Задает кольцевой буфер, доступный процессам
typedef struct {
    CycleBuffer buffer; // буфер для заполнения ячеек
    int have_reader; // индикатор наличия читателя
    int reader_pid; // идентификатор процесса читателя
    // int have_writer;      // индикатор наличия писателя
    int writer_pid; // идентификатор процесса писателя
} shared_memory;

void shared_memory_init(shared_memory* shm);

// имя области разделяемой памяти
extern const char* shar_object;
extern int buf_id; // дескриптор объекта памяти
extern shared_memory* shm; // указатель на разделямую память, хранящую буфер

// имя семафора для занятых ячеек
extern const char* full_sem_name;
extern sem_t* full; // указатель на семафор занятых ячеек

// имя семафора для свободных ячеек
extern const char* empty_sem_name;
extern sem_t* empty; // указатель на семафор свободных ячеек

// имя семафора (мьютекса) для критической секции,
// обеспечивающей доступ к буферу
extern const char* mutex_sem_name;
extern sem_t* mutex; // указатель на семафор читателей

// Имя семафора для управления доступом.
// Позволяет читателю дождаться разрешения от читателя.
// Даже если читатель стартовал первым
extern const char* admin_sem_name;
extern sem_t* admin; // указатель на семафор читателей

// Функция осуществляющая при запуске общие манипуляции с памятью и семафорами
// для децентрализованно подключаемых процессов читателей и писателей.
void init(void);

// Функция закрывающая семафоры общие для писателей и читателей
void close_common_semaphores(void);

// Функция, удаляющая все семафоры и разделяемую память
void unlink_all(void);
