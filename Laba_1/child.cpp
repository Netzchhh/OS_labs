#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: no file name\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: can't open file  " << argv[1] << "\n";
        return 1;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        float num, sum = 0.0;
        bool hasNumber = false;
        while (iss >> num) {
            sum += num;
            hasNumber = true;
        }
        if (hasNumber)
            std::cout << sum << std::endl;
    }
    file.close();
    return 0;
}