#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <pthread.h>
#include <sstream>
#include <iomanip>
using namespace std;
const int RUN = 32;
int* global_arr = nullptr;  // Глобальный массив
int global_n = 0;
int global_max_threads = 0;
int temp_val = 0; // Счётчик для присвоения индексов потокам
// Функция для удаления пробелов из строки (trim).
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}
// Функция для получения времени
double get_baseline(int n) {
    ifstream file("1.txt");
    if (!file.is_open()) return -1.0;
    string line;
    while (getline(file, line)) {
        if (line.find("|") == string::npos) continue;
        stringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, '|')) {
            tokens.push_back(trim(token));
        }
        if (tokens.size() >= 5 &&  // | n | p | T | S | E |
            tokens[1] == to_string(n) &&
            tokens[2] == "1") {
            return stod(tokens[3]);  // T из строки с p=1.
        }
    }
    return -1.0;  // Не найдено.
}
// Функция сортировки вставками для малых блоков (runs) в TimSort.

void insertionSort(int arr[], int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int t = arr[i]; 
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
    // Сортируем малые блоки вставками
    for (int i = left; i <= right; i += RUN)
        insertionSort(arr, i, min(i + RUN - 1, right));
    // Слияние блоков
    for (int s = RUN; s < n; s = 2 * s)// s - размер блока для слияния
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

// Вывод данных для проверки эффективности
void append_to_readme(int n, int p, double T, double S, double E) {
    ofstream file("1.txt", ios::app);
    if (!file.is_open()) {
        file.close();
        ofstream new_file("1.txt");
        new_file << "# Результаты экспериментов\n\n"
                 << "| Размер массива (n) | Потоки (p) | Время (T, с) | Ускорение (S) | Эффективность (E) |\n"
                 << "|---------------------|------------|--------------|---------------|-------------------|\n";
        new_file.close();
        file.open("1.txt", ios::app);
    }

    // 2 знака после запятой
    file << "| " << n << " | " << p << " | "
         << fixed << setprecision(3) << T << " | "
         << fixed << setprecision(2) << S << " | "
         << fixed << setprecision(2) << E << " |\n";
    file.close();
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

    // Вычисление S и E.
    double S = 1.0;
    double E = 1.0;
    double T1 = get_baseline(n);
    if (T1 < 0) cout << "Warning: No baseline for n=" << n << endl;
    if (T1 > 0 && max_threads > 1) {
        S = T1 / time_taken;
        E = S / max_threads;
    } else if (max_threads == 1) {
        S = 1.0;
        E = 1.0;
    }

    // Запись в 1.txt (при каждом запуске).
    append_to_readme(n, max_threads, time_taken, S, E);

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
