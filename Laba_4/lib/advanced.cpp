#include "../include/contract.h"
#include <algorithm>
#include <vector>

extern "C" {

// Алгоритм Евклида для НОД
int GCF(int A, int B) {
    if (A <= 0 || B <= 0) return 0;
    
    while (B != 0) {
        int temp = B;
        B = A % B;
        A = temp;
    }
    
    return A;
}

// Быстрая сортировка Хоара
void quickSort(int* array, int low, int high) {
    if (low < high) {
        int pivot = array[(low + high) / 2];
        int i = low;
        int j = high;
        
        while (i <= j) {
            while (array[i] < pivot) i++;
            while (array[j] > pivot) j--;
            
            if (i <= j) {
                int temp = array[i];
                array[i] = array[j];
                array[j] = temp;
                i++;
                j--;
            }
        }
        
        if (low < j) quickSort(array, low, j);
        if (i < high) quickSort(array, i, high);
    }
}

int* Sort(int* array, int size) {
    if (!array || size <= 0) return array;
    
    quickSort(array, 0, size - 1);
    return array;
}

}
