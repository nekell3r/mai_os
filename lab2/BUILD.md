# Инструкции по сборке

## Требования

- CMake версии 3.10 или выше
- Ninja build system
- C++ компилятор с поддержкой C++17:
  - Windows: MSVC (Visual Studio 2017 или новее) или MinGW

## Сборка на Windows с Ninja

### Основной способ (Ninja)

```powershell
# Создать директорию для сборки
mkdir build
cd build

# Сгенерировать файлы сборки Ninja
cmake -G Ninja ..

# Собрать проект
ninja

# Или альтернативно через cmake
# cmake --build .

# Запустить
cd src
.\median_filter.exe -h
```

### Сборка Release версии

```powershell
mkdir build
cd build

# Сгенерировать с оптимизациями
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..

# Собрать
ninja

# Запустить
cd src
.\median_filter.exe -h
```

### Сборка Debug версии

```powershell
mkdir build
cd build

# Сгенерировать с отладочной информацией
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..

# Собрать
ninja

# Запустить
cd src
.\median_filter.exe -h
```

## Установка Ninja (если не установлен)

### Через Chocolatey
```powershell
choco install ninja
```

### Через Scoop
```powershell
scoop install ninja
```

### Вручную
1. Скачайте с https://github.com/ninja-build/ninja/releases
2. Распакуйте `ninja.exe` в директорию в PATH

### Проверка установки
```powershell
ninja --version
```

## Примечание

Данная программа разработана для Windows и использует WinAPI для работы с потоками.

## Тестовый запуск

После успешной сборки выполните тестовый запуск:

```powershell
# Из директории build\src (для Ninja)
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
# Из директории build/
# Скрипт автоматически найдет исполняемый файл
copy ..\test_performance.ps1 .
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

### "ninja: command not found" или "'ninja' is not recognized"

Установите Ninja (см. раздел "Установка Ninja" выше).

### "Cannot open file median_filter.exe"

Убедитесь, что вы находитесь в правильной директории:
- Для Ninja: `build\src\`

### "CMake не найден"

Установите CMake с https://cmake.org/download/

### Ошибки компиляции MSVC

Убедитесь, что вы запускаете команды из **Developer Command Prompt for VS** или **Developer PowerShell for VS**, где настроены переменные окружения для MSVC компилятора.

## Пересборка

Для пересборки проекта после изменений:

```powershell
# Из директории build/
ninja

# Или для очистки и полной пересборки
ninja clean
ninja
```

## Полная очистка и пересборка

Чтобы пересобрать проект с нуля:

```powershell
# Из директории проекта
cd ..
rmdir /s /q build

# Создать заново
mkdir build
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

## Быстрая сборка

```powershell
# Создать алиас для быстрой сборки (опционально)
function Build-Lab2 {
    Push-Location $PSScriptRoot
    if (!(Test-Path "build")) { mkdir build }
    cd build
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
    ninja
    Pop-Location
}
```

