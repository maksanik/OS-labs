#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>

void* thread_function(void* arg) {
time_t now;
time(&now);
printf("Поток id: %lu, родительский pid: %d, время: %s", pthread_self(), getpid(), ctime(&now));
return NULL;
}

int main() {
pid_t pid1, pid2;
time_t now;

printf("Родительский процесс pid: %d\n", getpid());

pid1 = fork();
if (pid1 == 0) {
// Первый дочерний процесс
time(&now);
printf("Первый дочерний процесс pid: %d, родительский pid: %d, время: %s", getpid(), getppid(), ctime(&now));
printf("Информация о портах и соединениях:\n");
//system("netstat -tuln"); // Вызов system для вывода информации о портах и соединениях
} else if (pid1 > 0) {
pid2 = fork();
if (pid2 == 0) {
// Второй дочерний процесс
time(&now);
printf("Второй дочерний процесс pid: %d, родительский pid: %d, время: %s", getpid(), getppid(), ctime(&now));

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
printf("Родительский процесс после fork() pid: %d, время: %s", getpid(), ctime(&now));
//system("ps -x"); // Выполняем команду ps -x
wait(NULL); // Ожидаем завершения дочерних процессов
wait(NULL);
}
} else {
printf("Ошибка вызова fork, потомок не создан\n");
}

return 0;
}