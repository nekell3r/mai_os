# Лабораторная работа №1 - Межпроцессное взаимодействие (Pipes)

**Вариант 10:** Родитель отправляет числа детям через pipe. Дети проверяют: отрицательное/простое → завершение, составное → возврат.

---

## Сборка

```powershell
mkdir build; cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```
---

## Запуск

```powershell
cd build\src
copy ..\..\test_numbers.txt .

# Интерактивно
.\parent.exe
# Введите имя файла: test_numbers.txt

# Или через pipe
echo test_numbers.txt | .\parent.exe
```

---

## Примеры

### test_numbers.txt
```
15
20
8
12
7
```

### Вывод
```
Введите имя файла: test_numbers.txt
16-00-21MSG parent Читаю файл 'test_numbers.txt'
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

---

## Алгоритм

**Родитель:**
1. Читает имя файла
2. Открывает файл
3. Читает числа построчно
4. Отправляет через pipe дитю
5. Получает ответы

**Дитя:**
1. Читает числа из stdin (pipe)
2. Проверяет:
   - Отрицательное → EXIT
   - Простое → EXIT
   - Составное → отправить родителю
3. Завершение программы

---

## Windows API функции

- `CreateProcess()` — создание дочернего процесса
- `CreatePipe()` — создание канала
- `ReadFile()` / `WriteFile()` — чтение/запись
- `WaitForSingleObject()` — ожидание процесса
- `GetExitCodeProcess()` — код возврата
- `SetHandleInformation()` — наследование дескрипторов
- `GetStdHandle()` — стандартные дескрипторы

---

## Трассировка (WinDbg)

```powershell
windbg .\build\src\parent.exe
```

В WinDbg:
```
# Просмотр загруженных модулей
lm

# Включить логирование
.logopen D:\programming_projects\mai_os\lab1\build\src\trace.log

# Breakpoint на CreatePipe (использует NtCreateFile)
bu ntdll!NtCreateFile ".echo === NtCreateFile (CreatePipe) ===; g"

# Breakpoint на CreateProcess
bu ntdll!NtCreateUserProcess ".echo === NtCreateUserProcess (CreateProcessW) ===; g"

# Breakpoint на запись в pipe
bu ntdll!NtWriteFile ".echo NtWriteFile; g"

# Breakpoint на чтение из pipe
bu ntdll!NtReadFile ".echo NtReadFile; g"

# Запустить программу (нужно ввести имя файла)
g

# После завершения
.logclose
```

### Интерпретация трассировки

| Системный вызов | Что означает |
|-----------------|-----------|
| **NtCreateFile** | CreatePipe создает канал (вызывает это 2 раза для 2 pipe'ов) |
| **NtCreateUserProcess** | CreateProcess создает дочерний процесс |
| **NtWriteFile** | Запись в pipe (родитель → дитя или дитя → родитель) |
| **NtReadFile** | Чтение из pipe |
