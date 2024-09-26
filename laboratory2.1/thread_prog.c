#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void* thread_function(void* arg) {
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    printf("Поток id: %lu, родительский pid: %d, время: %02d:%02d:%02d\n", pthread_self(), getpid(), local->tm_hour, local->tm_min, local->tm_sec);
    return NULL;
}

int main() {
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    printf("Второй дочерний процесс (замещён exec) pid: %d, родительский pid: %d, время: %02d:%02d:%02d\n", getpid(), getppid(), local->tm_hour, local->tm_min, local->tm_sec);

    // Создаём два потока
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, thread_function, NULL);
    pthread_create(&thread2, NULL, thread_function, NULL);

    // Ожидаем завершения потоков
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}