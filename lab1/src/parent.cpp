#include "../common/comm.h"
#include <fstream>
#include <string>

int main() {
    // Запрос имени файла у пользователя
    std::string filename;
    std::cout << "Введите имя файла: ";
    std::getline(std::cin, filename);
    
    // Открываем файл
    std::ifstream file(filename);
    if (!file.is_open()) {
        LogErr("parent", "Не удалось открыть файл: " + filename);
        return 1;
    }
    
    LogMsg("parent", "Читаю файл '" + filename + "'");
    
    // Создаём pipe'ы для взаимодействия с дочерним процессом
    HANDLE pipe_to_child_read = nullptr;
    HANDLE pipe_to_child_write = nullptr;
    HANDLE pipe_from_child_read = nullptr;
    HANDLE pipe_from_child_write = nullptr;
    
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;  // ✅ По умолчанию НЕ наследуются
    sa.lpSecurityDescriptor = nullptr;
    
    BOOL pipe1_created = CreatePipe(&pipe_to_child_read, &pipe_to_child_write, &sa, 0);
    ASSERT_MSG(pipe1_created, "Не удалось создать pipe_to_child");
    if (!pipe1_created) {
        LogErr("parent", "Не удалось создать pipe_to_child");
        return 1;
    }
    
    BOOL pipe2_created = CreatePipe(&pipe_from_child_read, &pipe_from_child_write, &sa, 0);
    ASSERT_MSG(pipe2_created, "Не удалось создать pipe_from_child");
    if (!pipe2_created) {
        LogErr("parent", "Не удалось создать pipe_from_child");
        CloseHandle(pipe_to_child_read);
        CloseHandle(pipe_to_child_write);
        return 1;
    }
    
    // ✅ Явно говорим: "эти 2 дескриптора НАСЛЕДУЙ"
    BOOL inherit1 = SetHandleInformation(pipe_to_child_read, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    ASSERT_MSG(inherit1, "Не удалось установить наследование pipe_to_child_read");
    
    BOOL inherit2 = SetHandleInformation(pipe_from_child_write, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    ASSERT_MSG(inherit2, "Не удалось установить наследование pipe_from_child_write");
    
    // Настраиваем параметры запуска дочернего процесса
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = pipe_to_child_read;
    si.hStdOutput = pipe_from_child_write;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE); // stderr остаётся в консоли
    
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    
    // Запускаем дочерний процесс
    char cmd[] = "child.exe";
    BOOL process_created = CreateProcess(nullptr, cmd, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi);
    ASSERT_MSG(process_created, "Не удалось создать дочерний процесс child.exe");
    if (!process_created) {
        std::stringstream error;
        error << "Не удалось создать дочерний процесс. Код ошибки: 0x" << std::hex << GetLastError();
        LogErr("parent", error.str());
        CloseHandle(pipe_to_child_read);
        CloseHandle(pipe_to_child_write);
        CloseHandle(pipe_from_child_read);
        CloseHandle(pipe_from_child_write);
        return 1;
    }
    
    // Закрываем ненужные концы pipe'ов в родительском процессе
    CloseHandle(pipe_to_child_read);    // дочерний процесс читает из этого конца
    CloseHandle(pipe_from_child_write); // дочерний процесс пишет в этот конец
    
    // Читаем файл построчно и отправляем числа дочернему процессу
    std::string line;
    bool should_terminate = false;
    
    while (std::getline(file, line) && !should_terminate) {
        // Пропускаем пустые строки
        if (line.empty()) continue;
        
        try {
            int number = std::stoi(line);
            
            std::stringstream msg;
            msg << "Отправляю число " << number << " дочернему процессу";
            LogMsg("parent", msg.str());
            
            // Отправляем число дочернему процессу
            DWORD bytes_written;
            BOOL write_success = WriteFile(pipe_to_child_write, &number, sizeof(int), &bytes_written, nullptr);
            ASSERT_MSG(write_success && bytes_written == sizeof(int), "Ошибка записи числа в pipe");
            if (!write_success || bytes_written != sizeof(int)) {
                LogErr("parent", "Ошибка записи в pipe");
                break;
            }
            
            // Читаем ответ от дочернего процесса
            int response;
            DWORD bytes_read;
            if (!ReadFile(pipe_from_child_read, &response, sizeof(int), &bytes_read, nullptr)) {
                LogErr("parent", "Ошибка чтения из pipe");
                break;
            }
            
            if (bytes_read == sizeof(int)) {
                if (response == -1) {
                    // Дочерний процесс сигнализирует о завершении
                    LogMsg("parent", "Получен сигнал завершения от дочернего процесса");
                    should_terminate = true;
                } else if (response > 0) {
                    // Дочерний процесс вернул составное число
                    std::stringstream msg_resp;
                    msg_resp << "Получено составное число " << response << " от дочернего процесса";
                    LogMsg("parent", msg_resp.str());
                }
            } else if (bytes_read == 0) {
                // Дочерний процесс закрыл pipe
                LogMsg("parent", "Дочерний процесс завершился");
                break;
            }
            
        } catch (const std::exception& e) {
            std::stringstream err;
            err << "Ошибка преобразования строки в число: " << line;
            LogErr("parent", err.str());
        }
    }
    
    // Закрываем файл и pipe'ы
    file.close();
    CloseHandle(pipe_to_child_write);
    CloseHandle(pipe_from_child_read);
    
    // Ожидаем завершения дочернего процесса
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    std::stringstream exit_msg;
    exit_msg << "Дочерний процесс завершился с кодом " << exitCode;
    LogMsg("parent", exit_msg.str());
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    LogMsg("parent", "Завершение работы");
    return 0;
}

