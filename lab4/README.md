# Лабораторная работа №4 - Динамические библиотеки

**Вариант 9:** Сортировка целочисленного массива
- Метод 1: Пузырьковая сортировка
- Метод 2: Сортировка Хоара (QuickSort)

---

## Структура проекта

```
lab4/
├── common/              # Общие модули (из base)
├── src/
│   ├── sort_interface.h    # Интерфейс (контракт) функции Sort
│   ├── bubble_sort.cpp     # Реализация пузырьковой сортировки (DLL)
│   ├── quicksort.cpp       # Реализация QuickSort (DLL)
│   ├── program1.cpp        # Программа со статической линковкой
│   └── program2.cpp         # Программа с динамической загрузкой
├── CMakeLists.txt
└── README.md
```

---

## Сборка

```powershell
mkdir build
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

После сборки:
- `bubble_sort.dll` и `quicksort.dll` в `build/Release/`
- `program1.exe` и `program2.exe` в `build/Release/`

---

## Использование

### Program 1 (Статическая линковка)

```powershell
cd build\Release
.\program1.exe
```

**Команды:**
- `1 <num1> <num2> ... <numN>` - Сортировать массив
- `exit` - Выход

**Пример:**
```
> 1 5 2 8 1 3
Input array: 5 2 8 1 3
Sorted array: 1 2 3 5 8
```

### Program 2 (Динамическая загрузка)

```powershell
cd build\Release
.\program2.exe
```

**Команды:**
- `0` - Переключить между bubble_sort.dll и quicksort.dll
- `1 <num1> <num2> ... <numN>` - Сортировать массив
- `exit` - Выход

**Пример:**
```
> 0
Switched to: QuickSort (Hoare)
> 1 5 2 8 1 3
Input array: 5 2 8 1 3
Sorted array: 1 2 3 5 8
> 0
Switched to: Bubble Sort
```

---

## Реализация

### Интерфейс (sort_interface.h)

```cpp
extern "C" {
    int* Sort(int* array, int size);
}
```

### Bubble Sort (bubble_sort.cpp)

Классическая пузырьковая сортировка - O(n²).

### QuickSort (quicksort.cpp)

Сортировка Хоара (Hoare partition scheme) - O(n log n) в среднем.

---

## Системные вызовы Windows API

### Program 2 использует:

- **LoadLibraryA()** - загрузка DLL в память
- **GetProcAddress()** - получение адреса функции
- **FreeLibrary()** - выгрузка DLL

### Трассировка (WinDbg):

```powershell
cd lab4
windbg .\build\program2.exe

# Загрузка символов
.reload
ld ntdll

# DLL Loading (Native API - NTDLL)
# LdrLoadDll вызывается из LoadLibraryA
bp ntdll!LdrLoadDll ".echo === [LOAD] LdrLoadDll (LoadLibraryA) ===; g"
# LdrGetProcedureAddress вызывается из GetProcAddress
bp ntdll!LdrGetProcedureAddress ".echo === [PROC] LdrGetProcedureAddress (GetProcAddress) ===; g"
# LdrUnloadDll вызывается из FreeLibrary
bp ntdll!LdrUnloadDll ".echo === [FREE] LdrUnloadDll (FreeLibrary) ===; g"

# Memory mapping для DLL
bp ntdll!NtOpenSection ".echo === [OPEN] NtOpenSection (DLL) ===; g"
bp ntdll!NtMapViewOfSection ".echo === [MAP] NtMapViewOfSection (DLL) ===; g"
bp ntdll!NtUnmapViewOfSection ".echo === [UNMAP] NtUnmapViewOfSection (DLL) ===; g"

# I/O
bp ntdll!NtReadFile ".echo === [READ] NtReadFile (console input) ===; g"
bp ntdll!NtWriteFile ".echo === [WRITE] NtWriteFile (console output) ===; g"

# Cleanup
bp ntdll!NtClose ".echo === [CLOSE] NtClose ===; g"

.logopen trace_lab4_full.log
g
```

**Примечание:** Используются функции NTDLL (Native API), которые вызываются из высокоуровневых функций kernel32:
- `LdrLoadDll` ← вызывается из `LoadLibraryA`
- `LdrGetProcedureAddress` ← вызывается из `GetProcAddress`
- `LdrUnloadDll` ← вызывается из `FreeLibrary`

Это более надежный способ, так как символы NTDLL всегда доступны в WinDbg.

**Пример лога трассировки:**

```
... (initialization - загрузка системных DLL: advapi32.dll, msvcrt.dll, sechost.dll, RPCRT4.dll и др.) ...

=== [WRITE] NtWriteFile (console output) ===
=== [WRITE] NtWriteFile (console output) ===
... (вывод приветственного сообщения) ...

# Initial DLL load (bubble_sort.dll)
=== [LOAD] LdrLoadDll (LoadLibraryA) ===
=== [OPEN] NtOpenSection (DLL) ===
=== [MAP] NtMapViewOfSection (DLL) ===
ModLoad: ... bubble_sort.dll
=== [PROC] LdrGetProcedureAddress (GetProcAddress) ===
=== [WRITE] NtWriteFile (console output) ===
... (вывод "Library loaded: bubble_sort.dll") ...

# User input: "0" (switch DLL)
=== [READ] NtReadFile (console input) ===
=== [FREE] LdrUnloadDll (FreeLibrary) ===
=== [UNMAP] NtUnmapViewOfSection (DLL) ===
=== [CLOSE] NtClose ===
=== [LOAD] LdrLoadDll (LoadLibraryA) ===
=== [OPEN] NtOpenSection (DLL) ===
=== [MAP] NtMapViewOfSection (DLL) ===
ModLoad: ... quicksort.dll
=== [PROC] LdrGetProcedureAddress (GetProcAddress) ===
=== [WRITE] NtWriteFile (console output) ===
... (вывод "Switched to: QuickSort (Hoare)") ...

# User input: "1 5 2 8 1 3" (sort array)
=== [READ] NtReadFile (console input) ===
=== [WRITE] NtWriteFile (console output) ===
... (вывод "Input array: 5 2 8 1 3") ...
... (sorting happens in DLL code - без системных вызовов) ...
=== [WRITE] NtWriteFile (console output) ===
... (вывод "Sorted array: 1 2 3 5 8") ...

# User input: "exit"
=== [READ] NtReadFile (console input) ===
=== [FREE] LdrUnloadDll (FreeLibrary) ===
=== [UNMAP] NtUnmapViewOfSection (DLL) ===
=== [CLOSE] NtClose ===
=== [WRITE] NtWriteFile (console output) ===
... (вывод "Exiting...") ...
```

**Примечания к трассировке:**

1. **Инициализация процесса:** В начале лога много вызовов `LdrLoadDll` и `NtMapViewOfSection` для загрузки системных DLL (advapi32.dll, msvcrt.dll и др.). Это нормально и связано с инициализацией процесса Windows.

2. **Загрузка пользовательской DLL:** При загрузке `bubble_sort.dll` или `quicksort.dll` видно:
   - `LdrLoadDll` - загрузка DLL
   - `NtOpenSection` - открытие секции DLL
   - `NtMapViewOfSection` - маппинг DLL в память (с ModLoad сообщением)
   - `LdrGetProcedureAddress` - получение адреса функции `Sort`

3. **Выполнение сортировки:** Сама сортировка происходит в коде DLL без дополнительных системных вызовов - это обычный код, выполняющийся в адресном пространстве процесса.

4. **Выгрузка DLL:** При переключении или завершении:
   - `LdrUnloadDll` - выгрузка DLL
   - `NtUnmapViewOfSection` - отмена маппинга
   - `NtClose` - закрытие дескрипторов

---

## Сравнение подходов

| Аспект | Статическая линковка | Динамическая загрузка |
|--------|---------------------|----------------------|
| Время линковки | Компиляция | Выполнение |
| Размер exe | Больше | Меньше |
| Гибкость | Низкая | Высокая |
| Системные вызовы | Нет | LoadLibrary, GetProcAddress |
| Зависимости | Все в exe | DLL должны быть доступны |

