#include <dlfcn.h>
#include <iostream>
#include <string>
#include <sstream>
#include "../include/contract.h"

void* lib = nullptr;
GCFFunc GCFPtr = nullptr;
SortFunc SortPtr = nullptr;

bool loadLib(const char* path) {
    if (lib)
        dlclose(lib);

    lib = dlopen(path, RTLD_LAZY);
    if (!lib) {
        std::cerr << "dlopen error: " << dlerror() << "\n";
        return false;
    }

    GCFPtr = (GCFFunc)dlsym(lib, "GCF");
    SortPtr = (SortFunc)dlsym(lib, "Sort");

    if (!GCFPtr || !SortPtr) {
        std::cerr << "dlsym error\n";
        return false;
    }

    std::cout << "Loaded: " << path << "\n";
    return true;
}

int main() {
    std::cout << "Dynamic loading program\n";
    std::cout << "Commands:\n";
    std::cout << "0 - switch between basic and advanced libraries\n";
    std::cout << "1 A B - calculate GCD of A and B\n";
    std::cout << "2 size n1 n2 ... - sort array of given size\n";
    std::cout << "Enter empty line to exit\n";

    if (!loadLib("./libbasic.so")) {
        std::cerr << "Failed to load initial library\n";
        return 1;
    }

    std::string line;
    
    while (true) {
        std::cout << "> ";
        
        // Читаем всю строку
        if (!std::getline(std::cin, line)) {
            // Ctrl+D или конец файла
            std::cout << "\nExiting program...\n";
            break;
        }
        
        // Проверяем на пустую строку
        if (line.empty()) {
            std::cout << "Exiting program...\n";
            break;
        }
        
        // Используем stringstream для парсинга
        std::stringstream ss(line);
        int cmd;
        ss >> cmd;
        
        if (ss.fail()) {
            std::cout << "Invalid command. Use 0, 1 or 2.\n";
            continue;
        }

        if (cmd == 0) {
            static bool toggle = false;
            toggle = !toggle;
            loadLib(toggle ? "./libadvanced.so" : "./libbasic.so");
        } else if (cmd == 1) {
            int A, B;
            if (!(ss >> A >> B)) {
                std::cout << "Invalid arguments for GCD. Use: 1 A B\n";
                continue;
            }
            std::cout << "GCD = " << GCFPtr(A, B) << "\n";
        } else if (cmd == 2) {
            int size;
            if (!(ss >> size)) {
                std::cout << "Invalid size\n";
                continue;
            }
            
            if (size <= 0) {
                std::cout << "Size must be positive\n";
                continue;
            }
            
            int* array = new int[size];
            bool valid = true;
            for (int i = 0; i < size; i++) {
                if (!(ss >> array[i])) {
                    std::cout << "Not enough elements for array. Expected " << size << " numbers.\n";
                    valid = false;
                    break;
                }
            }
            
            if (valid) {
                SortPtr(array, size);
                std::cout << "Sorted array: ";
                for (int i = 0; i < size; i++) {
                    std::cout << array[i] << " ";
                }
                std::cout << "\n";
            }
            
            delete[] array;
        } else {
            std::cout << "Unknown command. Use 0, 1 or 2.\n";
        }
    }
    
    // Освобождаем библиотеку при выходе
    if (lib) {
        dlclose(lib);
    }
    
    return 0;
}
