#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

void* thread_function(void* arg) {
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    printf("Поток id: %lu, родительский pid: %d, время: %02d:%02d:%02d\n", pthread_self(), getpid(), local->tm_hour, local->tm_min, local->tm_sec);
}

int main() {
    pid_t pid1, pid2;
    time_t now;
    int status;

    printf("Родительский процесс pid: %d\n", getpid());
    pid1 = fork();
    wait(&status);

    if (pid1 == 0) { // Первый дочерний процесс
        time(&now);
        struct tm *local = localtime(&now);
        printf("Первый дочерний процесс pid: %d, родительский pid: %d, время: %02d:%02d:%02d\n", getpid(), getppid(),  local->tm_hour, local->tm_min, local->tm_sec);
        printf("Информация о портах и соединениях:\n");

        system("ss -tuln"); // Вызов system для вывода информации о портах и соединениях

    } else if (pid1 > 0) {
        pid2 = fork();
        wait(&status);

        if (pid2 == 0) { // Второй дочерний процесс
            time(&now);
            struct tm *local = localtime(&now);
            printf("Второй дочерний процесс pid: %d, родительский pid: %d, время: %02d:%02d:%02d\n", getpid(), getppid(), local->tm_hour, local->tm_min, local->tm_sec);

            // Создаём два потока
            pthread_t thread1, thread2;
            pthread_create(&thread1, NULL, thread_function, NULL);
            pthread_create(&thread2, NULL, thread_function, NULL);

            // Ожидаем завершения потоков
            pthread_join(thread1, NULL);
            pthread_join(thread2, NULL);
        } else {
            // Родительский процесс
            time(&now);
            struct tm *local = localtime(&now);
            printf("Родительский процесс после fork() pid: %d, время: %02d:%02d:%02d\n", getpid(), local->tm_hour, local->tm_min, local->tm_sec);

            system("ps -x");
        }
    }
    return 0;
}