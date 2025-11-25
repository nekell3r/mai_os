#include "../common/comm.h"
#include <fstream>
#include <string>

// Структура для обмена данными через memory-mapped file
struct SharedData {
    int number;              // Число для проверки
    int response;            // Ответ от дочернего процесса
    bool has_request;        // Есть ли новый запрос от родителя
    bool has_response;       // Есть ли новый ответ от ребёнка
    bool should_terminate;   // Флаг завершения работы
};

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
    
    // Создаём file mapping объект для shared memory
    const char* mapping_name = "Local\\Lab3SharedMemory";
    HANDLE hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,    // Используем системную память (не файл)
        nullptr,                 // Атрибуты безопасности по умолчанию
        PAGE_READWRITE,          // Права на чтение и запись
        0,                       // Старшие 32 бита размера
        sizeof(SharedData),      // Младшие 32 бита размера
        mapping_name             // Имя объекта
    );
    
    ASSERT_MSG(hMapFile != nullptr, "Не удалось создать file mapping");
    if (hMapFile == nullptr) {
        LogErr("parent", "Не удалось создать file mapping");
        return 1;
    }
    
    // Отображаем file mapping в адресное пространство процесса
    SharedData* pSharedData = static_cast<SharedData*>(MapViewOfFile(
        hMapFile,               // Дескриптор file mapping
        FILE_MAP_ALL_ACCESS,    // Права доступа
        0,                      // Старшие 32 бита смещения
        0,                      // Младшие 32 бита смещения
        sizeof(SharedData)      // Размер отображения
    ));
    
    ASSERT_MSG(pSharedData != nullptr, "Не удалось отобразить view of file");
    if (pSharedData == nullptr) {
        LogErr("parent", "Не удалось отобразить view of file");
        CloseHandle(hMapFile);
        return 1;
    }
    
    // Инициализируем shared memory
    pSharedData->number = 0;
    pSharedData->response = 0;
    pSharedData->has_request = false;
    pSharedData->has_response = false;
    pSharedData->should_terminate = false;
    
    LogMsg("parent", "Shared memory инициализирована");
    
    // Создаём события для синхронизации
    const char* event_request_name = "Local\\Lab3EventRequest";
    const char* event_response_name = "Local\\Lab3EventResponse";
    
    HANDLE hEventRequest = CreateEventA(
        nullptr,                // Атрибуты безопасности по умолчанию
        FALSE,                  // Auto-reset event
        FALSE,                  // Начальное состояние - non-signaled
        event_request_name      // Имя события
    );
    ASSERT_MSG(hEventRequest != nullptr, "Не удалось создать event для запросов");
    
    HANDLE hEventResponse = CreateEventA(
        nullptr,                // Атрибуты безопасности по умолчанию
        FALSE,                  // Auto-reset event
        FALSE,                  // Начальное состояние - non-signaled
        event_response_name     // Имя события
    );
    ASSERT_MSG(hEventResponse != nullptr, "Не удалось создать event для ответов");
    
    LogMsg("parent", "События для синхронизации созданы");
    
    // Настраиваем параметры запуска дочернего процесса
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    
    // Запускаем дочерний процесс
    char cmd[] = "child.exe";
    BOOL process_created = CreateProcess(nullptr, cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    ASSERT_MSG(process_created, "Не удалось создать дочерний процесс child.exe");
    if (!process_created) {
        std::stringstream error;
        error << "Не удалось создать дочерний процесс. Код ошибки: 0x" << std::hex << GetLastError();
        LogErr("parent", error.str());
        UnmapViewOfFile(pSharedData);
        CloseHandle(hMapFile);
        CloseHandle(hEventRequest);
        CloseHandle(hEventResponse);
        return 1;
    }
    
    LogMsg("parent", "Дочерний процесс запущен");
    
    // Даём дочернему процессу время на инициализацию
    Sleep(100);
    
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
            
            // Записываем число в shared memory
            pSharedData->number = number;
            pSharedData->has_request = true;
            pSharedData->has_response = false;
            
            // Сигнализируем дочернему процессу о новом запросе
            SetEvent(hEventRequest);
            
            // Ожидаем ответа от дочернего процесса (таймаут 5 секунд)
            DWORD wait_result = WaitForSingleObject(hEventResponse, 5000);
            
            if (wait_result == WAIT_OBJECT_0) {
                // Получен ответ
                if (pSharedData->has_response) {
                    int response = pSharedData->response;
                    
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
                    
                    pSharedData->has_response = false;
                }
            } else if (wait_result == WAIT_TIMEOUT) {
                LogErr("parent", "Таймаут ожидания ответа от дочернего процесса");
                break;
            } else {
                LogErr("parent", "Ошибка ожидания события");
                break;
            }
            
        } catch (const std::exception& e) {
            std::stringstream err;
            err << "Ошибка преобразования строки в число: " << line;
            LogErr("parent", err.str());
        }
    }
    
    // Сигнализируем дочернему процессу о завершении работы
    pSharedData->should_terminate = true;
    SetEvent(hEventRequest);
    
    // Закрываем файл
    file.close();
    
    // Ожидаем завершения дочернего процесса
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    std::stringstream exit_msg;
    exit_msg << "Дочерний процесс завершился с кодом " << exitCode;
    LogMsg("parent", exit_msg.str());
    
    // Освобождаем ресурсы
    UnmapViewOfFile(pSharedData);
    CloseHandle(hMapFile);
    CloseHandle(hEventRequest);
    CloseHandle(hEventResponse);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    LogMsg("parent", "Завершение работы");
    return 0;
}

