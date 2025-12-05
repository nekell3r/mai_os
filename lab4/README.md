# Лабораторная работа №4 - Динамические библиотеки

**Задача 1:** Расчет интеграла функции sin(x) на отрезке [A, B] с шагом e
- Метод 1: Метод прямоугольников
- Метод 2: Метод трапеций

**Задача 2 (Вариант 9):** Сортировка целочисленного массива
- Метод 1: Пузырьковая сортировка
- Метод 2: Сортировка Хоара (QuickSort)

---

## Структура проекта

```
lab4/
├── common/              # Общие модули (из base)
├── src/
│   ├── integral_interface.h  # Интерфейс функции SinIntegral
│   ├── rectangles.cpp        # Реализация метода прямоугольников (DLL)
│   ├── trapezoids.cpp        # Реализация метода трапеций (DLL)
│   ├── sort_interface.h      # Интерфейс функции Sort
│   ├── bubble_sort.cpp       # Реализация пузырьковой сортировки (DLL)
│   ├── quicksort.cpp         # Реализация QuickSort (DLL)
│   ├── program1.cpp          # Программа со статической линковкой (интеграл)
│   └── program2.cpp          # Программа с динамической загрузкой (сортировка)
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
- `rectangles.dll` и `trapezoids.dll` в `build/`
- `bubble_sort.dll` и `quicksort.dll` в `build/`
- `program1.exe` и `program2.exe` в `build/`

---

## Использование

### Program 1 (Статическая линковка - Интеграл)

```powershell
cd build
.\program1.exe
```

**Команды:**
- `1 <A> <B> <e>` - Рассчитать интеграл sin(x) на [A, B] с шагом e (метод прямоугольников)
- `2 <A> <B> <e>` - Рассчитать интеграл sin(x) на [A, B] с шагом e (метод трапеций)
- `exit` - Выход

**Пример:**
```
> 1 0 3.14159 0.1
23-56-59MSG program1 Integral of sin(x) on [0, 3.14159] with step 0.1 (Rectangle method):
Result: 2.00012
> 2 0 3.14159 0.1
23-56-59MSG program1 Integral of sin(x) on [0, 3.14159] with step 0.1 (Trapezoidal method):
Result: 2.00006
```

### Program 2 (Динамическая загрузка - Сортировка)

```powershell
cd build
.\program2.exe
```

**Команды:**
- `0` - Переключить между bubble_sort.dll и quicksort.dll
- `1 <num1> <num2> ... <numN>` - Сортировать массив (Bubble Sort)
- `2 <num1> <num2> ... <numN>` - Сортировать массив (QuickSort)
- `exit` - Выход

**Пример:**
```
> 1 5 2 8 1 3
23-56-59MSG program2 Input array (Bubble Sort):
5 2 8 1 3
23-56-59MSG program2 Sorted array:
1 2 3 5 8
> 2 5 2 8 1 3
23-56-59MSG program2 Input array (QuickSort (Hoare)):
5 2 8 1 3
23-56-59MSG program2 Sorted array:
1 2 3 5 8
> 0
23-56-59MSG program2 Switched to: QuickSort (Hoare)
```

---

## Реализация

### Интерфейс интеграла (integral_interface.h)

```cpp
extern "C" {
    float SinIntegral(float A, float B, float e);
}
```

### Метод прямоугольников (rectangles.cpp)

Вычисление интеграла методом прямоугольников. Используется значение функции в середине каждого интервала.

### Метод трапеций (trapezoids.cpp)

Вычисление интеграла методом трапеций. Используется среднее значение функции на границах каждого интервала.

### Интерфейс сортировки (sort_interface.h)

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

| Аспект | Статическая линковка (Program 1) | Динамическая загрузка (Program 2) |
|--------|---------------------|----------------------|
| Задача | Интеграл sin(x) | Сортировка массива |
| Время линковки | Компиляция | Выполнение |
| Размер exe | Больше (код включен) | Меньше (только ссылки) |
| Гибкость | Низкая (фиксированная реализация) | Высокая (переключение реализаций) |
| Системные вызовы | Нет загрузки DLL | LoadLibrary, GetProcAddress, FreeLibrary |
| Зависимости | Все в exe | DLL должны быть доступны |
| Команды | "1" и "2" для разных методов | "0" для переключения, "1" и "2" для разных методов |

