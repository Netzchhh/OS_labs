// main.cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <pthread.h>

using namespace std;

const int RUN = 32;
int* global_arr = nullptr;  // Глобальный массив
int global_n = 0;
int global_max_threads = 0;
int temp_val = 0; // Счётчик для присвоения индексов потокам

// Функция сортировки вставками для малых блоков (runs) в TimSort.
void insertionSort(int arr[], int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int t = arr[i]; // Текущий элемент который мы вставляем
        int j = i - 1;
        // Сдвиг элементов
        while (j >= left && t < arr[j]) {
            arr[j + 1] = arr[j]; 
            j--;
        }
        arr[j + 1] = t;
    }
}

// Функция слияния двух отсортированных подмассивов в TimSort
// l/m/r - границы левого, середины и правого подмассивов
void merge(int arr[], int l, int m, int r) {
    int len1 = m - l + 1, len2 = r - m;
    int* left = new int[len1]; // временный левый массив
    int* right = new int[len2]; // временный правый массив
    for (int i = 0; i < len1; i++)
        left[i] = arr[l + i];
    for (int i = 0; i < len2; i++)
        right[i] = arr[m + 1 + i];
    int i = 0;
    int j = 0;
    int k = l;// Индекс в основном массиве
    // Слияние: берём меньший элемент из левого или правого
    while (i < len1 && j < len2) {
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        } else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }
    // Копируем оставшиеся элементы левого подмассива
    while (i < len1) {
        arr[k] = left[i];
        k++;
        i++;
    }
    // Копируем оставшиеся элементы правого подмассива
    while (j < len2) {
        arr[k] = right[j];
        k++;
        j++;
    }
    delete[] left; // Освобождаем память для следующего кода
    delete[] right;
}

// Основная функция TimSort для подмассива 
void timSortSub(int arr[], int left, int right) {
    int n = right - left + 1;
    // Сортируем малые блоки (runs) вставками.
    for (int i = left; i <= right; i += RUN)
        insertionSort(arr, i, min(i + RUN - 1, right));
    // Слияние блоков (как в merge sort, удваивая размер)
    for (int s = RUN; s < n; s = 2 * s)// s - текущий размер блока для слияния.
        for (int l = left; l <= right; l += 2 * s) { // Начло слияния
            int mid = l + s - 1; // Конец левой части
            int r = min(l + 2 * s - 1, right); // Конец правой части
            if (mid < r)
                merge(arr, l, mid, r);
        }
}

// Функция для потока, сортирующего свою часть массива
void* thread_sort(void* arg) {
    int index = temp_val++; // Получаем индекс потока
    // Разделение массива на части: равномерно распределяем элементы между потоками.
    int part_size = (global_n + global_max_threads - 1) / global_max_threads;
    int left = index * part_size;
    int right = min(left + part_size - 1, global_n - 1);
    timSortSub(global_arr, left, right);  // 
    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <max_threads> <array_size>" << endl;
        return 1;
    }
    int max_threads = atoi(argv[1]);
    int n = atoi(argv[2]);
    global_n = n;
    global_max_threads = max_threads;

    // Генератор случайных вводных данных
    global_arr = new int[n];
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < n; i++) {
        global_arr[i] = rand() % 10000; // Числа от 0 до 9999
    }

    ofstream log("trace.log");
    log << "Starting TimSort with " << max_threads << " threads on array of size " << n << endl;

    clock_t start = clock();
    // Создание массива потоков (pthread_t - дескрипторы).
    pthread_t* threads = new pthread_t[max_threads];
    temp_val = 0;
    for (int i = 0; i < max_threads; i++) { // Цикл создания потоков
        pthread_create(&threads[i], nullptr, thread_sort, nullptr); // &threads[i] - дескриптор, nullptr - атрибуты по умолчанию, thread_sort - функция
    }
    for (int i = 0; i < max_threads; i++) {
        pthread_join(threads[i], nullptr);
    }

    // Финальное слияние
    int part_size = (n + max_threads - 1) / max_threads;
    for (int s = part_size; s < n; s = 2 * s) {
        for (int left = 0; left < n; left += 2 * s) {
            int mid = left + s - 1;
            int right = min(left + 2 * s - 1, n - 1);
            if (mid < right) {
                merge(global_arr, left, mid, right); // Слияние пар блоков
            }
        }
    }
    //] Замеряем потраченное время
    double time_taken = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
    log << "Sorting completed in " << time_taken << " seconds" << endl;
    // Проверка на верность сортировки
    bool sorted = true;
    for (int i = 1; i < n; i++) {
        if (global_arr[i] < global_arr[i-1]) { sorted = false; break; }
    }
    cout << "Array is " << (sorted ? "" : "not ") << "sorted." << endl;

    delete[] global_arr;
    delete[] threads;
    return 0;
}