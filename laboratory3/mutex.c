#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5
#define INCREMENTS_PER_THREAD 1000

int counter = 0; // Общая переменная
pthread_mutex_t mutex; // Мьютекс для защиты критической секции

void* increment_counter(void* arg) {
for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
pthread_mutex_lock(&mutex); // Захватываем мьютекс перед изменением
counter++; // Изменяем общую переменную
pthread_mutex_unlock(&mutex); // Освобождаем мьютекс после изменения
}
return NULL;
}

int main() {
pthread_t threads[NUM_THREADS];
pthread_mutex_init(&mutex, NULL); // Инициализируем мьютекс

// Создаем потоки
for (int i = 0; i < NUM_THREADS; i++) {
pthread_create(&threads[i], NULL, increment_counter, NULL);
}

// Ожидаем завершения всех потоков
for (int i = 0; i < NUM_THREADS; i++) {
pthread_join(threads[i], NULL);
}

printf("Итоговое значение счётчика: %d\n", counter);

pthread_mutex_destroy(&mutex); // Уничтожаем мьютекс
return 0;
}