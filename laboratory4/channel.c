#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MAX 10

void printMatrix(float mat[MAX][MAX], int n);
int gaussJordan(float mat[MAX][MAX], float inv[MAX][MAX], int n);
int readMatrixFromFile(const char *filename, float mat[MAX][MAX], int *n);
int gaussPipe(float augmented[MAX][MAX * 2], int n);
int gaussMem(float augmented[MAX][MAX * 2], int n);
int gaussSocket(float augmented[MAX][MAX * 2], int n);

char* curr_method;

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Не введён метод\n");
        return 0;
    } 
    curr_method = argv[1];
    printf("Введён метод: [%s] \n", curr_method);

    int n;
    float mat[MAX][MAX], inv[MAX][MAX];

    const char *filename = "matrix.txt"; // Имя файла с матрицей

    // Чтение матрицы из файла
    if (readMatrixFromFile(filename, mat, &n) != 0) {
        printf("Ошибка при чтении матрицы из файла.\n");
        return 1;
    }

    // Вычисление обратной матрицы
    if (gaussJordan(mat, inv, n) == 0) {
        printf("Обратная матрица:\n");
        printMatrix(inv, n);
    } else {
        printf("Матрица необратима.\n");
    }

    return 0;
}


void printMatrix(float mat[MAX][MAX], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%0.3f ", mat[i][j]);
        }
        printf("\n");
    }
}


int gaussJordan(float mat[MAX][MAX], float inv[MAX][MAX], int n) {
    float temp;
    float augmented[MAX][MAX * 2];

    // Создание расширенной матрицы [A | I]
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmented[i][j] = mat[i][j];
            augmented[i][j + n] = (i == j) ? 1 : 0;
        }
    }

    if (strcmp(curr_method, "pipe") == 0) {
        gaussPipe(augmented, n);
    } else if (strcmp(curr_method, "mem") == 0) {
        gaussMem(augmented, n);
    } else if (strcmp(curr_method, "socket") == 0){
        gaussSocket(augmented, n);
    }

    // Извлекаем обратную матрицу из расширенной
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inv[i][j] = augmented[i][j + n];
        }
    }

    return 0;
}

int gaussPipe(float augmented[MAX][MAX * 2], int n) {
    int pipefds[2];
    pid_t pid;
    float temp;

    for (int i = 0; i < n; i++) {
        pipe(pipefds);
        pid = fork();
        if (pid == 0) {

            // Если элемент на главной диагонали равен нулю, меняем строки
            if (augmented[i][i] == 0) {
            int j;

                for (j = i + 1; j < n; j++) {
                    if (augmented[j][i] != 0) {
                        // Меняем строки
                        for (int k = 0; k < 2 * n; k++) {
                            temp = augmented[i][k];
                            augmented[i][k] = augmented[j][k];
                            augmented[j][k] = temp;
                        }
                        break;
                    }
                }
                if (j == n) {
                    return 1; // Матрица необратима
                }
            }

        // Нормализуем текущую строку
            temp = augmented[i][i];
            for (int j = 0; j < 2 * n; j++) {
                augmented[i][j] /= temp;
            }

        // Обнуляем элементы в столбце
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    temp = augmented[j][i];
                    for (int k = 0; k < 2 * n; k++) {
                        augmented[j][k] -= augmented[i][k] * temp;
                    }
                }
            }

            write(pipefds[1], augmented, sizeof(float) * MAX * 2 * n);
            close(pipefds[1]);
            exit(0);

        } else if (pid > 1) {
            wait(NULL);

            read(pipefds[0], augmented, sizeof(float) * MAX * 2 * n);
            close(pipefds[0]);
        }
    }

    return 0;
}

int gaussMem(float augmented[MAX][MAX * 2], int n) {
    const char *shm_name = "/augmented_matrix"; // Имя разделяемой памяти
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); // Создание/открытие разделяемой памяти

    // Устанавливаем размер разделяемой памяти
    ftruncate(shm_fd, sizeof(float) * MAX * 2 * n);

    // Отображаем разделяемую память в адресное пространство процесса
    float *shared_augmented = mmap(NULL, sizeof(float) * MAX * 2 * n, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    
    if (shared_augmented == MAP_FAILED) {
        perror("mmap failed");
        return -1;
    }

    pid_t pid;
    float temp;

    for (int i = 0; i < n; i++) {
        pid = fork();
        if (pid == 0) { // Дочерний процесс

            // Если элемент на главной диагонали равен нулю, меняем строки
            if (augmented[i][i] == 0) {
                int j;
                for (j = i + 1; j < n; j++) {
                    if (augmented[j][i] != 0) {
                        // Меняем строки
                        for (int k = 0; k < 2 * n; k++) {
                            temp = augmented[i][k];
                            augmented[i][k] = augmented[j][k];
                            augmented[j][k] = temp;
                        }
                        break;
                    }
                }
                if (j == n) {
                    return 1; // Матрица необратима
                }
            }

            // Нормализуем текущую строку
            temp = augmented[i][i];
            for (int j = 0; j < 2 * n; j++) {
                augmented[i][j] /= temp;
            }

            // Обнуляем элементы в столбце
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    temp = augmented[j][i];
                    for (int k = 0; k < 2 * n; k++) {
                        augmented[j][k] -= augmented[i][k] * temp;
                    }
                }
            }

            // Копируем данные в разделяемую память
            memcpy(shared_augmented, augmented, sizeof(float) * MAX * 2 * n);

            exit(0); // Выход из дочернего процесса

        } else if (pid > 0) {
        wait(NULL);

        // Копируем данные обратно в augmented
        memcpy(augmented, shared_augmented, sizeof(float) * MAX * 2 * n);
        }
    }

    // Очистка разделяемой памяти
    munmap(shared_augmented, sizeof(float) * MAX * 2 * n);

    // Удаляем разделяемую память
    shm_unlink(shm_name);

    return 0;
}

int gaussSocket(float augmented[MAX][MAX * 2], int n) {
    int sockfds[2];
    pid_t pid;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    float temp;

    // Создаем сокет
    socketpair(AF_UNIX, SOCK_STREAM, 0, sockfds);

    for (int i = 0; i < n; i++) {
        pid = fork();
        if (pid == 0) { // Дочерний процесс
            // Если элемент на главной диагонали равен нулю, меняем строки
            if (augmented[i][i] == 0) {
                int j;
                for (j = i + 1; j < n; j++) {
                    if (augmented[j][i] != 0) {
                        // Меняем строки
                        for (int k = 0; k < 2 * n; k++) {
                            temp = augmented[i][k];
                            augmented[i][k] = augmented[j][k];
                            augmented[j][k] = temp;
                        }
                        break;
                    }
                }
                if (j == n) {
                    return 1; // Матрица необратима
                }
            }

            // Нормализуем текущую строку
            temp = augmented[i][i];
            for (int j = 0; j < 2 * n; j++) {
                augmented[i][j] /= temp;
            }

            // Обнуляем элементы в столбце
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    temp = augmented[j][i];
                    for (int k = 0; k < 2 * n; k++) {
                        augmented[j][k] -= augmented[i][k] * temp;
                    }
                }
            }

            // Отправляем данные через сокет
            write(sockfds[1], augmented, sizeof(float) * MAX * 2 * n);
            close(sockfds[1]);
            exit(0);
        } else if (pid > 0) {
            wait(NULL);

            // Читаем данные из сокета
            read(sockfds[0], augmented, sizeof(float) * MAX * 2 * n);
            close(sockfds[0]);
        }
    }

    return 0;
}

int readMatrixFromFile(const char *filename, float mat[MAX][MAX], int *n) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        return 1; // Ошибка при открытии файла
    }

    // Чтение размера матрицы
    fscanf(file, "%d", n);

    // Чтение матрицы из файла
    for (int i = 0; i < *n; i++) {
        for (int j = 0; j < *n; j++) {
            fscanf(file, "%f", &mat[i][j]);
        }
    }

    fclose(file);

    return 0; // Успешное чтение
}