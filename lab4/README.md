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

```
bp kernel32!LoadLibraryA ".echo === LoadLibraryA ===; g"
bp kernel32!GetProcAddress ".echo === GetProcAddress ===; g"
bp kernel32!FreeLibrary ".echo === FreeLibrary ===; g"
```

---

## Сравнение подходов

| Аспект | Статическая линковка | Динамическая загрузка |
|--------|---------------------|----------------------|
| Время линковки | Компиляция | Выполнение |
| Размер exe | Больше | Меньше |
| Гибкость | Низкая | Высокая |
| Системные вызовы | Нет | LoadLibrary, GetProcAddress |
| Зависимости | Все в exe | DLL должны быть доступны |

