# Инструкции по сборке

## Требования

- CMake версии 3.10 или выше
- C++ компилятор с поддержкой C++17:
  - Windows: MSVC (Visual Studio 2017 или новее) или MinGW
  - Linux/Unix: GCC 7+ или Clang 5+

## Сборка на Windows

### Вариант 1: Visual Studio

```powershell
# Создать директорию для сборки
mkdir build
cd build

# Сгенерировать проект Visual Studio
cmake ..

# Собрать проект
cmake --build . --config Release

# Запустить
cd src\Release
.\median_filter.exe -h
```

### Вариант 2: MinGW

```powershell
# Создать директорию для сборки
mkdir build
cd build

# Сгенерировать Makefile
cmake -G "MinGW Makefiles" ..

# Собрать
mingw32-make

# Запустить
cd src
.\median_filter.exe -h
```

### Вариант 3: NMake (из Visual Studio Command Prompt)

```cmd
mkdir build
cd build
cmake -G "NMake Makefiles" ..
nmake
cd src
median_filter.exe -h
```

## Примечание

Данная программа разработана для Windows и использует WinAPI для работы с потоками.

## Тестовый запуск

После успешной сборки выполните тестовый запуск:

```powershell
# Из директории build\src\Release или build\src
.\median_filter.exe -f ..\..\test_matrix.txt -w 3 -k 1 -t 2 -p
```

Вы должны увидеть вывод с исходной и результирующей матрицами, а также статистику выполнения.

## Тестирование производительности

### Пример 1: Сравнение разного количества потоков

```powershell
# 1 поток
.\median_filter.exe -g 1000 1000 -w 5 -k 3 -t 1

# 4 потока
.\median_filter.exe -g 1000 1000 -w 5 -k 3 -t 4

# 8 потоков
.\median_filter.exe -g 1000 1000 -w 5 -k 3 -t 8
```

### Автоматическое тестирование

```powershell
# Скопируйте скрипт в директорию build/
copy ..\test_performance.ps1 .

# Запустите
.\test_performance.ps1
```

### Пример 2: Разные размеры матриц

```powershell
# Маленькая матрица
.\median_filter.exe -g 100 100 -w 5 -k 3 -t 4

# Средняя матрица
.\median_filter.exe -g 500 500 -w 5 -k 3 -t 4

# Большая матрица
.\median_filter.exe -g 2000 2000 -w 5 -k 3 -t 4
```

## Возможные проблемы

### Windows: "Cannot open file median_filter.exe"

Убедитесь, что вы находитесь в правильной директории:
- Для Visual Studio: `build\src\Release\`
- Для MinGW: `build\src\`

### "CMake не найден"

Установите CMake с https://cmake.org/download/

## Отладочная сборка

Для отладки используйте Debug конфигурацию:

```powershell
# Visual Studio
cmake --build . --config Debug

# MinGW
cmake -DCMAKE_BUILD_TYPE=Debug ..
mingw32-make
```

## Очистка

Чтобы пересобрать проект с нуля:

```powershell
# Удалить директорию сборки
cd ..
rmdir /s build

# Создать заново
mkdir build
cd build
cmake ..
```

