# Лабораторная работа №3 - Вариант 10

## Описание задания

Программа реализует межпроцессное взаимодействие между родительским и дочерним процессами через **file mapping (memory-mapped files)** на Windows.

### Алгоритм работы:

1. **Родительский процесс:**
   - Запрашивает у пользователя имя файла
   - Создаёт shared memory через file mapping
   - Создаёт события (Events) для синхронизации
   - Запускает дочерний процесс
   - Читает числа из файла и отправляет их через shared memory
   - Получает ответы от дочернего процесса

2. **Дочерний процесс:**
   - Открывает существующий file mapping
   - Открывает события для синхронизации
   - Читает числа из shared memory
   - Проверяет каждое число:
     - Если число **отрицательное** → завершает работу (и родитель тоже завершается)
     - Если число **простое** → завершает работу (и родитель тоже завершается)
     - Если число **составное** → отправляет его обратно родителю через shared memory

## Структура проекта

```
lab3/
├── common/                 # Общие утилиты
│   ├── CMakeLists.txt
│   ├── defines.h           # Платформенные макросы
│   ├── comm.h              # Логирование (LogMsg, LogErr)
│   ├── errors.h            # Обработка ошибок
│   ├── errors.hpp          # Шаблоны для ошибок
│   └── errors.cpp          # Реализация DebugBreak
├── src/                    # Исходники программ
│   ├── CMakeLists.txt
│   ├── parent.cpp          # Родительский процесс
│   └── child.cpp           # Дочерний процесс
├── CMakeLists.txt          # Корневой файл сборки
├── test_numbers.txt        # Тестовый файл с числами
├── BUILD.md                # Инструкции по сборке
└── README.md               # Этот файл
```

## Требования

- **Windows 10/11**
- **CMake 3.10+**
- **Visual Studio 2019+** (или MinGW)
- **C++17**

## Компиляция и запуск

См. [BUILD.md](BUILD.md)

## Пример использования

### Тестовый файл `test_numbers.txt` содержит:
```
15
20
8
12
7
```

### Ожидаемый вывод:
```
Введите имя файла: test_numbers.txt
16-00-21MSG parent Читаю файл 'test_numbers.txt'
16-00-21MSG parent Shared memory инициализирована
16-00-21MSG parent События для синхронизации созданы
16-00-21MSG parent Дочерний процесс запущен
16-00-21MSG parent Отправляю число 15 дочернему процессу
16-00-21MSG parent Получено составное число 15 от дочернего процесса
16-00-21MSG parent Отправляю число 20 дочернему процессу
16-00-21MSG parent Получено составное число 20 от дочернего процесса
16-00-21MSG parent Отправляю число 8 дочернему процессу
16-00-21MSG parent Получено составное число 8 от дочернего процесса
16-00-21MSG parent Отправляю число 12 дочернему процессу
16-00-21MSG parent Получено составное число 12 от дочернего процесса
16-00-21MSG parent Отправляю число 7 дочернему процессу
16-00-21MSG parent Получен сигнал завершения от дочернего процесса
16-00-21MSG parent Дочерний процесс завершился с кодом 0
16-00-21MSG parent Завершение работы
```

## Windows API

Использованные Windows API функции:

### File Mapping:
- `CreateFileMappingA()` — создание объекта file mapping в памяти
- `OpenFileMappingA()` — открытие существующего объекта file mapping
- `MapViewOfFile()` — отображение file mapping в адресное пространство процесса
- `UnmapViewOfFile()` — отмена отображения

### Синхронизация:
- `CreateEventA()` — создание именованного события
- `OpenEventA()` — открытие существующего именованного события
- `SetEvent()` — установка события в signaled состояние
- `WaitForSingleObject()` — ожидание события

### Процессы:
- `CreateProcess()` — создание дочернего процесса
- `WaitForSingleObject()` — ожидание завершения дочернего процесса
- `GetExitCodeProcess()` — получение кода возврата процесса
- `CloseHandle()` — закрытие дескрипторов

## Особенности реализации

### 1. File Mapping (Memory-Mapped Files)

Вместо pipe'ов используется **shared memory** через механизм file mapping:

```cpp
// Родитель создаёт file mapping
HANDLE hMapFile = CreateFileMappingA(
    INVALID_HANDLE_VALUE,    // Используем системную память
    nullptr,                 
    PAGE_READWRITE,          
    0,                       
    sizeof(SharedData),      
    "Local\\Lab3SharedMemory"
);

// Ребёнок открывает существующий file mapping
HANDLE hMapFile = OpenFileMappingA(
    FILE_MAP_ALL_ACCESS,
    FALSE,
    "Local\\Lab3SharedMemory"
);
```

### 2. Структура Shared Memory

```cpp
struct SharedData {
    int number;              // Число для проверки
    int response;            // Ответ от дочернего процесса
    bool has_request;        // Есть ли новый запрос от родителя
    bool has_response;       // Есть ли новый ответ от ребёнка
    bool should_terminate;   // Флаг завершения работы
};
```

### 3. Синхронизация через Events

Для координации доступа к shared memory используются два события:

- **EventRequest** — родитель сигнализирует ребёнку о новом запросе
- **EventResponse** — ребёнок сигнализирует родителю о готовности ответа

```cpp
// Родитель отправляет запрос
pSharedData->number = 42;
pSharedData->has_request = true;
SetEvent(hEventRequest);  // Сигнализируем ребёнку

// Ребёнок ожидает запроса
WaitForSingleObject(hEventRequest, INFINITE);
// Обрабатывает number
pSharedData->response = result;
pSharedData->has_response = true;
SetEvent(hEventResponse);  // Сигнализируем родителю
```

### 4. Преимущества File Mapping перед Pipes

| Аспект | Pipes (Lab1) | File Mapping (Lab3) |
|--------|--------------|---------------------|
| **Механизм** | Потоковая передача данных | Разделяемая память |
| **Скорость** | Копирование данных | Прямой доступ к памяти |
| **Сложность** | Проще в использовании | Требует синхронизации |
| **Объём данных** | Последовательный | Произвольный доступ |
| **Типичное использование** | Потоки данных | Разделяемые структуры |

### 5. Именование объектов

Используется префикс `Local\\` для именования объектов:
- `Local\\Lab3SharedMemory` — file mapping
- `Local\\Lab3EventRequest` — событие запроса
- `Local\\Lab3EventResponse` — событие ответа

Префикс `Local\\` гарантирует, что объекты создаются в локальном пространстве имён сеанса.

### 6. Обработка ошибок

- Проверка всех системных вызовов через `ASSERT_MSG()`
- Правильное освобождение ресурсов в обоих процессах
- Таймаут для WaitForSingleObject (защита от зависания)

## Диаграмма взаимодействия

```
┌──────────┐                              ┌──────────┐
│   User   │                              │   File   │
└────┬─────┘                              └────┬─────┘
     │ filename                                │
     │                                         │
     ▼                                         │
┌─────────────────┐                           │
│     Parent      │◄──────────────────────────┘
│   (parent.exe)  │         read numbers
└────────┬────────┘
         │
         │ CreateProcess()
         │ + CreateFileMapping()
         │ + CreateEvent()
         │
         ├────────────────────────────────────┐
         │                                    │
         ▼                                    ▼
┌─────────────────────────────┐    ┌───────────────────┐
│      Shared Memory          │    │      Events       │
│   (File Mapping)            │    │  - EventRequest   │
│                             │    │  - EventResponse  │
│  struct SharedData {        │    └────────┬──────────┘
│    int number;              │             │
│    int response;            │             │
│    bool has_request;        │             │
│    bool has_response;       │             │
│    bool should_terminate;   │             │
│  }                          │             │
└──────────┬──────────────────┘             │
           │                                │
           │     OpenFileMapping()          │
           │     OpenEvent()                │
           │                                │
           ▼                                ▼
    ┌─────────────────┐
    │      Child      │
    │   (child.exe)   │
    │                 │
    │  - check prime  │
    │  - check < 0    │
    └─────────────────┘
```

## Последовательность взаимодействия

```
Parent                          Child
  │
  ├─ CreateFileMapping()
  ├─ MapViewOfFile()
  ├─ CreateEvent(Request)
  ├─ CreateEvent(Response)
  ├─ CreateProcess() ─────────────→ │
  │                                  ├─ OpenFileMapping()
  │                                  ├─ MapViewOfFile()
  │                                  ├─ OpenEvent(Request)
  │                                  ├─ OpenEvent(Response)
  │                                  ├─ WaitForSingleObject(Request)
  │                                  │   [WAITING...]
  ├─ Write number to SharedData
  ├─ SetEvent(Request) ─────────────→ │
  ├─ WaitForSingleObject(Response)   ├─ Read number
  │   [WAITING...]                   ├─ Check prime
  │                                  ├─ Write response to SharedData
  │                                  ├─ SetEvent(Response)
  │                                  └─ WaitForSingleObject(Request)
  │                                      [WAITING...]
  ├─ Read response ←──────────────────┘
  ├─ Write next number
  └─ ...
```

## Возможные проблемы и решения

### 1. child.exe не найден

**Проблема:** `Не удалось создать дочерний процесс`

**Решение:**
- Убедитесь, что `child.exe` находится в той же директории, что и `parent.exe`
- Проверьте, что оба файла скомпилированы в одной конфигурации

### 2. Не удалось открыть file mapping

**Проблема:** Child не может открыть shared memory

**Решение:**
- Убедитесь, что parent процесс запущен первым
- Проверьте, что имена объектов совпадают в обоих процессах
- Увеличьте Sleep() в parent после запуска child

### 3. Таймаут ожидания ответа

**Проблема:** `Таймаут ожидания ответа от дочернего процесса`

**Решение:**
- Проверьте, что child процесс корректно сигнализирует через SetEvent()
- Убедитесь, что флаги has_request/has_response корректно устанавливаются
- Увеличьте таймаут в WaitForSingleObject()

### 4. Ошибки доступа к памяти

**Проблема:** Access violation при работе с SharedData

**Решение:**
- Убедитесь, что MapViewOfFile() выполнен успешно в обоих процессах
- Проверьте, что размер SharedData одинаковый в parent и child
- Убедитесь, что UnmapViewOfFile() не вызывается преждевременно

## Сравнение с Lab1 и Lab2

| Аспект | Lab1 | Lab2 | Lab3 |
|--------|------|------|------|
| **Механизм IPC** | Pipes | Threads | File Mapping + Events |
| **Процессы** | Parent + Child | Single process | Parent + Child |
| **Синхронизация** | Pipe blocking | Mutex/Semaphore | Events |
| **Скорость** | Средняя | Быстрая | Очень быстрая |
| **Сложность** | Низкая | Средняя | Высокая |

## Дополнительная информация

### File Mapping vs Pipes

**File Mapping** подходит когда:
- Нужен быстрый доступ к общим данным
- Данные имеют структуру
- Процессы работают на одной машине

**Pipes** подходят когда:
- Нужна потоковая передача данных
- Большие объёмы данных
- Простая последовательная обработка

### Named Objects в Windows

Объекты с именами (named objects) в Windows позволяют разным процессам получить доступ к одному и тому же ресурсу:
- File mappings
- Events
- Mutexes
- Semaphores

Префиксы пространств имён:
- `Global\\` — доступно всем сеансам
- `Local\\` — доступно только текущему сеансу
- Без префикса — `Local\\` по умолчанию

### Auto-reset vs Manual-reset Events

В этой лабораторной используются **auto-reset events**:
- После WaitForSingleObject() автоматически переходят в non-signaled
- Не требуют вызова ResetEvent()
- Подходят для синхронизации "один запрос - один ответ"

**Manual-reset events** требуют явного вызова ResetEvent() и используются когда нужно разбудить несколько потоков одновременно.

## Заключение

Лабораторная работа №3 демонстрирует использование **file mapping** и **events** для межпроцессного взаимодействия на Windows. Это более производительный механизм по сравнению с pipes, но требует явной синхронизации.

**Ключевые навыки:**
- Работа с shared memory через CreateFileMapping/MapViewOfFile
- Синхронизация процессов через Events
- Именованные объекты Windows
- Обработка race conditions

