#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    std::string filename;
    std::cout << "Input file name: ";
    std::cin >> filename;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Дочерний процесс
        close(pipefd[0]); // закрываем конец для чтения

        // Nаправляем stdout в pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execl("./child", "child", filename.c_str(), (char*)NULL);
        perror("execl");
        return 1;
    } else {
        // Родительский процесс
        close(pipefd[1]); // закрываем запись
        char buffer[256];
        ssize_t count;
        std::cout << "\nResults:\n";
        while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[count] = '\0';
            std::cout << buffer;
        }
        close(pipefd[0]);
        wait(NULL);
    }
    return 0;
}