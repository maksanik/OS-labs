#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 5

sem_t semaphore;

void* thread_function(void* arg) {
int thread_id = *((int*)arg);
printf("Thread %d: Waiting to enter critical section...\n", thread_id);

// Захват семафора
sem_wait(&semaphore);

printf("Thread %d: Entered critical section.\n", thread_id);
sleep(1); // Имитация работы в критической секции
printf("Thread %d: Exiting critical section.\n", thread_id);

// Освобождение семафора
sem_post(&semaphore);

free(arg);
return NULL;
}

int main() {
pthread_t threads[NUM_THREADS];

// Инициализация семафора
sem_init(&semaphore, 0, 2); // 2 - количество разрешений

for (int i = 0; i < NUM_THREADS; i++) {
int* thread_id = malloc(sizeof(int));
*thread_id = i + 1;
pthread_create(&threads[i], NULL, thread_function, thread_id);
}

for (int i = 0; i < NUM_THREADS; i++) {
pthread_join(threads[i], NULL);
}

// Уничтожение семафора
sem_destroy(&semaphore);

return 0;
}