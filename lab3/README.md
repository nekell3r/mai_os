# Лабораторная работа №3 - Межпроцессное взаимодействие (File Mapping)

**Вариант 10:** Проверка чисел на простоту через shared memory.

---

## Сборка

```powershell
# Автоматически
.\rebuild.ps1

# Или вручную
mkdir build; cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

---

## Запуск

```powershell
cd src
copy ..\..\test_numbers.txt .
.\parent.exe
```

Программа запросит имя файла. Введите `test_numbers.txt` или путь к вашему файлу.

### Автоматический ввод

```powershell
echo test_numbers.txt | .\parent.exe
```

---

## Описание

Программа реализует межпроцессное взаимодействие между родительским и дочерним процессами через **file mapping (memory-mapped files)** на Windows.

### Алгоритм работы:

1. **Родительский процесс (parent.exe):**
   - Запрашивает имя файла с числами
   - Создаёт shared memory через file mapping
   - Создаёт события (Events) для синхронизации
   - Запускает дочерний процесс (child.exe)
   - Читает числа из файла и отправляет их через shared memory
   - Получает ответы от дочернего процесса

2. **Дочерний процесс (child.exe):**
   - Открывает существующий file mapping
   - Открывает события для синхронизации
   - Читает числа из shared memory
   - Проверяет каждое число:
     - Если число **отрицательное** → завершает работу
     - Если число **простое** → завершает работу
     - Если число **составное** → отправляет его обратно родителю

---

## Примеры

### Тестовый файл `test_numbers.txt`:

```
15
20
8
12
7
```

### Ожидаемый вывод:

```
Enter filename: test_numbers.txt
16-00-21MSG parent Reading file 'test_numbers.txt'
16-00-21MSG parent Shared memory initialized
16-00-21MSG parent Synchronization events created
16-00-21MSG parent Child process started
16-00-21MSG parent Sending number 15 to child process
16-00-21MSG parent Received composite number 15 from child process
16-00-21MSG parent Sending number 20 to child process
16-00-21MSG parent Received composite number 20 from child process
16-00-21MSG parent Sending number 8 to child process
16-00-21MSG parent Received composite number 8 from child process
16-00-21MSG parent Sending number 12 to child process
16-00-21MSG parent Received composite number 12 from child process
16-00-21MSG parent Sending number 7 to child process
16-00-21MSG parent Received termination signal from child process
16-00-21MSG parent Child process exited with code 0
16-00-21MSG parent Shutting down
```

### Создание своих тестов:

```powershell
# Тест с простым числом
echo 4 > test1.txt
echo 9 >> test1.txt
echo 11 >> test1.txt
echo test1.txt | .\parent.exe

# Тест с отрицательным числом
echo 10 > test2.txt
echo -5 >> test2.txt
echo test2.txt | .\parent.exe

# Тест только с составными числами
echo 4 > test3.txt
echo 6 >> test3.txt
echo 8 >> test3.txt
echo 9 >> test3.txt
echo 10 >> test3.txt
echo test3.txt | .\parent.exe
```

---

## Трассировка (WinDbg)

```powershell
windbg .\build\src\parent.exe
```

В WinDbg:

```
.logopen D:\programming_projects\mai_os\lab3\build\src\trace.log

# File Mapping
bp ntdll!NtCreateSection ".echo === [1] NtCreateSection ===; g"
bp ntdll!NtMapViewOfSection ".echo === [2] NtMapViewOfSection ===; g"
bp ntdll!NtUnmapViewOfSection ".echo === [UNMAP] NtUnmapViewOfSection ===; g"

# Events
bp ntdll!NtCreateEvent ".echo === [3] NtCreateEvent ===; g"
bp ntdll!NtOpenEvent ".echo === [CHILD] NtOpenEvent ===; g"
bp ntdll!NtSetEvent ".echo === [SIGNAL] NtSetEvent ===; g"
bp ntdll!NtWaitForSingleObject ".echo === [WAIT] NtWaitForSingleObject ===; g"

# Processes
bp ntdll!NtCreateUserProcess ".echo === [PROCESS] NtCreateUserProcess ===; g"

# I/O
bp ntdll!NtReadFile ".echo === [READ] NtReadFile ===; g"
bp ntdll!NtWriteFile ".echo === [WRITE] NtWriteFile ===; g"

g
.logclose
```

---

## Windows API

### File Mapping:
- `CreateFileMappingA()` — создание объекта file mapping
- `OpenFileMappingA()` — открытие существующего file mapping
- `MapViewOfFile()` — отображение в адресное пространство
- `UnmapViewOfFile()` — отмена отображения

### Синхронизация:
- `CreateEventA()` — создание именованного события
- `OpenEventA()` — открытие существующего события
- `SetEvent()` — установка события в signaled
- `WaitForSingleObject()` — ожидание события

### Процессы:
- `CreateProcess()` — создание дочернего процесса
- `WaitForSingleObject()` — ожидание завершения процесса
- `GetExitCodeProcess()` — получение кода возврата

---

## Структура проекта

```
lab3/
├── common/                 # Общие утилиты
│   ├── comm.h              # Логирование
│   ├── defines.h           # Платформенные макросы
│   └── errors.h/cpp/hpp    # Обработка ошибок
├── src/                    # Исходники
│   ├── parent.cpp          # Родительский процесс
│   └── child.cpp           # Дочерний процесс
├── test_numbers.txt        # Тестовый файл
├── rebuild.ps1             # Скрипт пересборки
└── README.md               # Этот файл
```
