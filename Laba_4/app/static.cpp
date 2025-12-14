#include <iostream>
#include <string>
#include <sstream>
#include "../include/contract.h"

int main() {
    std::cout << "Static linking program (using basic library)\n";
    std::cout << "Commands:\n";
    std::cout << "1 A B - calculate GCD of A and B\n";
    std::cout << "2 size n1 n2 ... - sort array of given size\n";
    std::cout << "Enter empty line to exit\n";

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
            std::cout << "Invalid command. Use 1 or 2.\n";
            continue;
        }

        if (cmd == 1) {
            int A, B;
            if (!(ss >> A >> B)) {
                std::cout << "Invalid arguments for GCD. Use: 1 A B\n";
                continue;
            }
            std::cout << "GCD = " << GCF(A, B) << "\n";
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
                Sort(array, size);
                std::cout << "Sorted array: ";
                for (int i = 0; i < size; i++) {
                    std::cout << array[i] << " ";
                }
                std::cout << "\n";
            }
            
            delete[] array;
        } else {
            std::cout << "Unknown command. Use 1 or 2.\n";
        }
    }
    
    return 0;
}
