#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// Функция проверки числа на простоту
bool is_prime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

int main() {
    int number;
    
    while (1) {
        // Читаем число из stdin (pipe от родителя)
        ssize_t bytes_read = read(STDIN_FILENO, &number, sizeof(int));
        
        if (bytes_read == 0) {
            // Родитель закрыл pipe
            break;
        }
        
        if (bytes_read != sizeof(int)) {
            perror("read");
            break;
        }
        
        fprintf(stderr, "Дочерний процесс: получено число %d\n", number);
        
        // Проверяем условия завершения
        if (number < 0) {
            fprintf(stderr, "Дочерний процесс: число отрицательное, завершаю работу\n");
            int response = -1;
            write(STDOUT_FILENO, &response, sizeof(int));
            exit(0);
        }
        
        if (is_prime(number)) {
            fprintf(stderr, "Дочерний процесс: число %d простое, завершаю работу\n", number);
            int response = -1;
            write(STDOUT_FILENO, &response, sizeof(int));
            exit(0);
        }
        
        // Число составное
        fprintf(stderr, "Дочерний процесс: число %d составное, отправляю родителю\n", number);
        write(STDOUT_FILENO, &number, sizeof(int));
    }
    
    fprintf(stderr, "Дочерний процесс: завершение работы\n");
    return 0;
}

