#include "../common/comm.h"
#include <cstdlib>

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
    LogMsg("child", "Дочерний процесс запущен");
    
    int number;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    
    ASSERT_MSG(hStdin != INVALID_HANDLE_VALUE, "Не удалось получить stdin handle");
    ASSERT_MSG(hStdout != INVALID_HANDLE_VALUE, "Не удалось получить stdout handle");
    
    while (true) {
        // Читаем число из stdin (pipe от родителя)
        DWORD bytes_read;
        BOOL read_success = ReadFile(hStdin, &number, sizeof(int), &bytes_read, nullptr);
        if (!read_success) {
            LogErr("child", "Ошибка чтения из pipe");
            break;
        }
        
        if (bytes_read == 0) {
            // Родитель закрыл pipe
            LogMsg("child", "Родитель закрыл pipe, завершаю работу");
            break;
        }
        
        ASSERT_MSG(bytes_read == sizeof(int) || bytes_read == 0, "Неполное чтение данных из pipe");
        if (bytes_read != sizeof(int)) {
            LogErr("child", "Неполное чтение данных");
            break;
        }
        
        std::stringstream msg;
        msg << "Получено число " << number;
        LogMsg("child", msg.str());
        
        // Проверяем условия завершения
        if (number < 0) {
            LogMsg("child", "Число отрицательное, завершаю работу");
            int response = -1;
            DWORD bytes_written;
            WriteFile(hStdout, &response, sizeof(int), &bytes_written, nullptr);
            return 0;
        }
        
        if (is_prime(number)) {
            std::stringstream msg_prime;
            msg_prime << "Число " << number << " простое, завершаю работу";
            LogMsg("child", msg_prime.str());
            int response = -1;
            DWORD bytes_written;
            WriteFile(hStdout, &response, sizeof(int), &bytes_written, nullptr);
            return 0;
        }
        
        // Число составное
        std::stringstream msg_composite;
        msg_composite << "Число " << number << " составное, отправляю родителю";
        LogMsg("child", msg_composite.str());
        DWORD bytes_written;
        BOOL write_success = WriteFile(hStdout, &number, sizeof(int), &bytes_written, nullptr);
        ASSERT_MSG(write_success && bytes_written == sizeof(int), "Ошибка записи составного числа в pipe");
    }
    
    LogMsg("child", "Завершение работы");
    return 0;
}

