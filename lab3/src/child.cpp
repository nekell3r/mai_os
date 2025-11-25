#include "../common/comm.h"
#include <cstdlib>

// Структура для обмена данными через memory-mapped file
struct SharedData {
    int number;              // Число для проверки
    int response;            // Ответ от дочернего процесса
    bool has_request;        // Есть ли новый запрос от родителя
    bool has_response;       // Есть ли новый ответ от ребёнка
    bool should_terminate;   // Флаг завершения работы
};

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
    
    // Открываем существующий file mapping объект
    const char* mapping_name = "Local\\Lab3SharedMemory";
    HANDLE hMapFile = OpenFileMappingA(
        FILE_MAP_ALL_ACCESS,    // Права доступа
        FALSE,                  // Не наследовать дескриптор
        mapping_name            // Имя объекта
    );
    
    ASSERT_MSG(hMapFile != nullptr, "Не удалось открыть file mapping");
    if (hMapFile == nullptr) {
        LogErr("child", "Не удалось открыть file mapping");
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
        LogErr("child", "Не удалось отобразить view of file");
        CloseHandle(hMapFile);
        return 1;
    }
    
    LogMsg("child", "Shared memory открыта");
    
    // Открываем события для синхронизации
    const char* event_request_name = "Local\\Lab3EventRequest";
    const char* event_response_name = "Local\\Lab3EventResponse";
    
    HANDLE hEventRequest = OpenEventA(
        EVENT_ALL_ACCESS,       // Права доступа
        FALSE,                  // Не наследовать дескриптор
        event_request_name      // Имя события
    );
    ASSERT_MSG(hEventRequest != nullptr, "Не удалось открыть event для запросов");
    
    HANDLE hEventResponse = OpenEventA(
        EVENT_ALL_ACCESS,       // Права доступа
        FALSE,                  // Не наследовать дескриптор
        event_response_name     // Имя события
    );
    ASSERT_MSG(hEventResponse != nullptr, "Не удалось открыть event для ответов");
    
    LogMsg("child", "События для синхронизации открыты");
    
    // Основной цикл обработки запросов
    while (true) {
        // Ожидаем сигнала от родительского процесса
        DWORD wait_result = WaitForSingleObject(hEventRequest, INFINITE);
        
        if (wait_result != WAIT_OBJECT_0) {
            LogErr("child", "Ошибка ожидания события");
            break;
        }
        
        // Проверяем флаг завершения
        if (pSharedData->should_terminate) {
            LogMsg("child", "Получен сигнал завершения, завершаю работу");
            break;
        }
        
        // Проверяем наличие нового запроса
        if (!pSharedData->has_request) {
            continue;
        }
        
        int number = pSharedData->number;
        pSharedData->has_request = false;
        
        std::stringstream msg;
        msg << "Получено число " << number;
        LogMsg("child", msg.str());
        
        // Проверяем условия завершения
        if (number < 0) {
            LogMsg("child", "Число отрицательное, завершаю работу");
            pSharedData->response = -1;
            pSharedData->has_response = true;
            SetEvent(hEventResponse);
            break;
        }
        
        if (is_prime(number)) {
            std::stringstream msg_prime;
            msg_prime << "Число " << number << " простое, завершаю работу";
            LogMsg("child", msg_prime.str());
            pSharedData->response = -1;
            pSharedData->has_response = true;
            SetEvent(hEventResponse);
            break;
        }
        
        // Число составное
        std::stringstream msg_composite;
        msg_composite << "Число " << number << " составное, отправляю родителю";
        LogMsg("child", msg_composite.str());
        
        pSharedData->response = number;
        pSharedData->has_response = true;
        
        // Сигнализируем родительскому процессу о готовности ответа
        SetEvent(hEventResponse);
    }
    
    // Освобождаем ресурсы
    UnmapViewOfFile(pSharedData);
    CloseHandle(hMapFile);
    CloseHandle(hEventRequest);
    CloseHandle(hEventResponse);
    
    LogMsg("child", "Завершение работы");
    return 0;
}

