#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> // Для wait()

int main() {
pid_t pid;

for (int i = 0; i < 10; i++) {
pid = fork(); // Создаем новый процесс

if (pid < 0) {
// Ошибка при создании процесса
perror("fork failed");
exit(1);
}

if (pid == 0) {
// Это дочерний процесс
printf("Дочерний процесс %d с PID %d\n", i + 1, getpid());
exit(0); // Завершаем дочерний процесс
}
}

// Ожидаем завершения всех дочерних процессов
for (int i = 0; i < 10; i++) {
wait(NULL);
}

return 0;
}