#include <iostream>
#include <cmath>
#include <pthread.h>
#include <vector>
#include <ctime>

using namespace std;

// Структура для передачі даних у потоки
struct ThreadData {
    double left;
    double right;
    int steps;
    double result;
};

// Функція для інтегрування
double function(double x) {
    return x * x * sin(x) + log(x + 1) + exp(-x);
}

// Функція для розрахунку інтегралу за методом прямокутників (права кінцева точка)
void* calculate_integral(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    double step_size = (data->right - data->left) / data->steps;
    double sum = 0.0;

    // Метод правих прямокутників
    for (int i = 1; i <= data->steps; ++i) {
        double x = data->left + i * step_size;
        sum += function(x) * step_size;
    }

    data->result = sum;
    pthread_exit(NULL);
}

int main() {
    double left, right, analytical_result;
    int num_threads, steps;

    // Вхідні дані від користувача
    cout << "Enter the left boundary of integration: ";
    cin >> left;

    cout << "Enter the right boundary of integration: ";
    cin >> right;

    cout << "Enter the number of threads: ";
    cin >> num_threads;

    cout << "Enter the number of integration steps: ";
    cin >> steps;

    cout << "Enter the analytical result of the integral: ";
    cin >> analytical_result;

    // Масив потоків і даних для кожного потоку
    pthread_t threads[num_threads];
    vector<ThreadData> thread_data(num_threads);

    double interval_length = (right - left) / num_threads;
    int steps_per_thread = steps / num_threads;

    clock_t start = clock(); // Початок вимірювання часу

    // Створення потоків
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].left = left + i * interval_length;
        thread_data[i].right = thread_data[i].left + interval_length;
        thread_data[i].steps = steps_per_thread;
        pthread_create(&threads[i], NULL, calculate_integral, (void*)&thread_data[i]);
    }

    double total_sum = 0.0;

    // Очікування завершення потоків і сумування результатів
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].result;
    }

    clock_t end = clock(); // Кінець вимірювання часу
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    // Обчислення відхилення від аналітичного значення
    double error = fabs(analytical_result - total_sum);

    // Виведення результатів
    cout << "The result of the integral: " << total_sum << endl;
    cout << "Analytical result: " << analytical_result << endl;
    cout << "Error: " << error << endl;
    cout << "Calculation time: " << time_spent << " seconds" << endl;

    return 0;
}
