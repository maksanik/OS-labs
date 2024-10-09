#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

pid_t child_pid;
int got_data = 0;
int counter = 0;

// Обработчик сигналов
void handle_signal(int signal) {
if (counter > 3) {
    printf("Не удалось получить данные. Завершение программы.\n");
    kill(child_pid, SIGKILL);
    exit(1);
}

if (signal == SIGUSR1) {
    printf("Данные успешно получены! Продолжение работы программы.\n");
    got_data = 1;
} else if (signal == SIGUSR2) {
    printf("Ошибка получения данных. Повторение попытки...\n");
    counter++;
}

}

int main() {
// Устанавливаем обработчики сигналов
signal(SIGUSR1, handle_signal);
signal(SIGUSR2, handle_signal);

// Создаем дочерний процесс
child_pid = fork();

if (child_pid < 0) {
perror("Ошибка при создании дочернего процесса");
exit(1);
}

// Дочерний процесс
if (child_pid == 0) {

printf("Дочерний процесс (PID: %d) работает...\n", getpid());

srand(time(NULL));

int rand_res = rand() % 4;
while (rand_res != 0)
{
    sleep(2);
    kill(getppid(), SIGUSR2);
    rand_res = rand() % 4;
}

sleep(2);
kill(getppid(), SIGUSR1);
printf("Дочерний процесс успешно завершает свою работу.\n");

exit(0);

} else {
// Родительский процесс
printf("Родительский процесс (PID: %d) ожидает успешного получения данных...\n", getpid());

while (got_data != 1) {
    pause();
}

sleep(2);

printf("Родительский процесс успешно завершает свою работу.\n");

}

return 0;
}