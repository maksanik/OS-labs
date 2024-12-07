#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>

void saveMemMap(pid_t pid, const char *output)
{
    char command[512];
    char filename[256];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    // Форматирование имени файла
    strftime(filename, sizeof(filename), "%Y-%m-%d_%H-%M-%S", tm_info);

    // Формирование команды
    int written = snprintf(command, sizeof(command), "pmap -x %d > %s/map_%d_%s", pid, output, pid, filename);

    // Выполнение команды
    if (system(command) == -1)
    {
        perror("Error executing system command");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Wrong number of arguments\n");
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);

    saveMemMap(pid, argv[argc - 1]);
    return 0;
}