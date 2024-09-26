#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>

void get_names(void);
void get_sys_info(void);
void get_sys_metrics(void);
void get_time_info(void);
void get_additional_inf(void);

int main() {

    get_names();
    get_sys_info();
    get_sys_metrics();
    get_time_info();
    get_additional_inf();

    return 0;
}

void get_names() {
    printf("Пользовательская информация:\n\n");
    char computer_name[30];
    gethostname(computer_name, 30);
    //
    char user_name[30];
    getlogin_r(user_name, 30);
    printf("Имя компьютера - %s\nИмя пользователя - %s\n----\n", computer_name, user_name);
}


void get_sys_info(void) {
    printf("Информация о системе:\n\n");
    struct utsname syst;
    uname(&syst);
    printf("Имя операционной системы - %s\n", syst.sysname);
    printf("Выпуск операционной системы - %s\n", syst.release);
    printf("Версия операционной системы - %s\n", syst.version);
    printf("----\n");
}

void get_sys_metrics(void) {
    printf("Системные метрики:\n\n");
    long metric;
    metric = sysconf(_SC_CHILD_MAX);
    printf("Максимально допустимое количество одновременно работающих процессов - %ld\n", metric);
    metric = sysconf(_SC_LOGIN_NAME_MAX);
    printf("Максимально допустимая длина имени пользователя - %ld\n", metric);
    metric = sysconf(_SC_OPEN_MAX);
    printf("Максимально допустимое количество файлов, которое одновременно может открыть процесс - %ld\n", metric);
    printf("----\n");
}

void get_time_info(void) {
    printf("Информация о времени:\n\n");

    time_t current_time = time(NULL);
    struct tm local_time;
    local_time = *localtime(&current_time);
    struct tm gm_time;
    gm_time = *gmtime(&current_time);

    printf("Текущее время в секундах - %ld\n", current_time);
    printf("Текущее местное время - %s", asctime(&local_time));
    printf("Время GMT - %s", asctime(&gm_time));
    printf("----\n");
}

void get_additional_inf(void) {
    printf("Дополнительные функции:\n\n");

    struct utsname syst;
    uname(&syst);
    printf("Архитектура команд процессора - %s\n", syst.machine);
    int pid = getpid();
    int ppid = getppid();
    printf("Идентификатор текущего процесса - %d\n", pid);
    printf("Идентификатор родительского процесса - %d\n", pid);

    char cwd[200];
    printf("Текущий каталог - %s\n", getcwd(cwd, 200));

    struct stat file_inf;
    stat("/home/maksanik/projects/OS-labs/laboratory1/main.c", &file_inf);
    printf("Размер исполняемого файла - %ld байт\n", file_inf.st_size);
}