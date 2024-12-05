#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define MAX_MEMORY 10000 // Максимальное количество выделенной памяти

// Глобальная переменная для хранения всех выделенных блоков памяти
void *allocated_memory[MAX_MEMORY];
int mem_index = 0; // Индекс для отслеживания выделенных блоков памяти

// Мьютекс для синхронизации доступа к общей памяти
pthread_mutex_t mem_mutex = PTHREAD_MUTEX_INITIALIZER;

// Обработчик сигнала для освобождения памяти
void handle_signal(int sig)
{
    printf("\nПолучен сигнал, освобождаю память...\n");
    pthread_mutex_lock(&mem_mutex);
    for (int i = 0; i < mem_index; i++)
    {
        free(allocated_memory[i]);
    }
    printf("Память освобождена. Завершение программы.\n");
    exit(0);
}

// Функция для задержки от 100 до 1000 наносекунд
void delay()
{
    struct timespec req = {0, (rand() % 901 + 100) * 1000}; // 100-1000 нс
    nanosleep(&req, NULL);
}

// Функция для работы в режиме 1 (malloc/free равное количество)
void *mode1(void *arg)
{
    while (1)
    {
        // Выделяем память
        pthread_mutex_lock(&mem_mutex);
        int *ptr = (int *)malloc(sizeof(int));
        if (ptr != NULL)
        {
            allocated_memory[mem_index++] = ptr;
            printf("Выделено %d блоков памяти\n", mem_index);
        }
        pthread_mutex_unlock(&mem_mutex);

        delay(); // Задержка

        // Освобождаем память
        pthread_mutex_lock(&mem_mutex);
        if (mem_index > 0)
        {
            free(allocated_memory[--mem_index]);
        }
        pthread_mutex_unlock(&mem_mutex);

        delay(); // Задержка
    }
    return NULL;
}

// Функция для работы в режиме 2 (выделение памяти больше, чем освобождение)
void *mode2(void *arg)
{
    while (1)
    {
        // Выделяем память
        pthread_mutex_lock(&mem_mutex);
        int *ptr = (int *)malloc(sizeof(int));
        if (ptr != NULL)
        {
            allocated_memory[mem_index++] = ptr;
            printf("Выделено %d блоков памяти\n", mem_index);
        }
        pthread_mutex_unlock(&mem_mutex);

        delay(); // Задержка

        // Освобождаем память с меньшей вероятностью
        pthread_mutex_lock(&mem_mutex);
        if (mem_index > 1 && rand() % 2 == 0)
        {
            free(allocated_memory[--mem_index]);
        }
        pthread_mutex_unlock(&mem_mutex);

        delay(); // Задержка
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Использование: %s <режим: 1 или 2>\n", argv[0]);
        return 1;
    }

    // Устанавливаем обработчик сигнала
    signal(SIGINT, handle_signal); // Обрабатываем сигнал прерывания (Ctrl+C)

    // pid_t pid = getpid();
    // printf("PID текущего процесса: %d \n", pid);

    // Инициализация случайного числа
    srand(time(NULL));

    pthread_t thread1;

    if (argv[1][0] == '1')
    {
        printf("Режим 1: выделение памяти malloc() равно освобождению памяти free()\n");

        // Создаем потоки для выделения и освобождения памяти
        pthread_create(&thread1, NULL, mode1, NULL);
        pthread_join(thread1, NULL);
    }
    else if (argv[1][0] == '2')
    {
        printf("Режим 2: выделение памяти больше malloc(), чем освобождение free()\n");

        // Создаем потоки для выделения и освобождения памяти
        pthread_create(&thread1, NULL, mode2, NULL);
        pthread_join(thread1, NULL);
    }
    else
    {
        printf("Неверный режим. Используйте 1 или 2.\n");
        return 1;
    }

    return 0;
}