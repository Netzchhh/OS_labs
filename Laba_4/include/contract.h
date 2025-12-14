#ifndef CONTRACT_H
#define CONTRACT_H

#include <cstddef>  // для size_t

typedef int (*GCFFunc)(int A, int B);
typedef int* (*SortFunc)(int* array, int size);

extern "C" {
    int GCF(int A, int B);
    int* Sort(int* array, int size);
}

#endif
