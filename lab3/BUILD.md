# Инструкция по сборке и запуску Lab3

## Требования

- **Windows 10/11**
- **CMake 3.10+**
- **Компилятор C++17:**
  - Visual Studio 2019+ (рекомендуется)
  - MinGW-w64
  - Clang для Windows

## Вариант 1: Сборка через CMake + Ninja (рекомендуется)

Ninja — это быстрый build-система, которая автоматически находит компилятор.

### Установка Ninja (если не установлен):

```powershell
# Через Chocolatey
choco install ninja

# Или скачайте с https://github.com/ninja-build/ninja/releases
```

### Сборка:

```powershell
# Перейдите в директорию lab3
cd D:\programming_projects\mai_os\lab3

# Создайте директорию для сборки
mkdir build
cd build

# Настройте проект (Ninja автоматически найдёт компилятор)
cmake .. -G Ninja

# Соберите проект
ninja

# Исполняемые файлы будут в build/src/
```

## Вариант 2: Сборка через Visual Studio

### Если у вас установлена Visual Studio:

```powershell
# Перейдите в директорию lab3
cd D:\programming_projects\mai_os\lab3

# Создайте директорию для сборки
mkdir build
cd build

# Настройте проект для Visual Studio
cmake .. -G "Visual Studio 17 2022"
# Для VS 2019: cmake .. -G "Visual Studio 16 2019"

# Соберите проект
cmake --build . --config Release

# Исполняемые файлы будут в build/src/Release/
```

## Вариант 3: Сборка через MinGW

### Если у вас установлен MinGW:

```powershell
# Перейдите в директорию lab3
cd D:\programming_projects\mai_os\lab3

# Создайте директорию для сборки
mkdir build
cd build

# Настройте проект для MinGW
cmake .. -G "MinGW Makefiles"

# Соберите проект
cmake --build .

# Исполняемые файлы будут в build/src/
```

## Запуск программы

### После успешной сборки:

```powershell
# Перейдите в директорию с исполняемыми файлами
cd src  # или cd src/Release для Visual Studio

# Скопируйте тестовый файл
copy ..\..\test_numbers.txt .

# Запустите parent.exe
.\parent.exe

# Программа попросит ввести имя файла:
# Введите: test_numbers.txt
```

### Или используйте echo для автоматического ввода:

```powershell
echo test_numbers.txt | .\parent.exe
```

## Ожидаемый вывод

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

## Структура после сборки

```
lab3/
├── build/
│   ├── src/
│   │   ├── parent.exe       ← Главный исполняемый файл
│   │   ├── child.exe        ← Дочерний процесс
│   │   └── test_numbers.txt ← Скопированный тестовый файл
│   └── ...
└── ...
```

## Быстрая пересборка

После изменения исходного кода:

### С Ninja:
```powershell
cd build
ninja
```

### С Visual Studio:
```powershell
cd build
cmake --build . --config Release
```

### С MinGW:
```powershell
cd build
cmake --build .
```

## Очистка проекта

```powershell
# Удалите директорию build
cd D:\programming_projects\mai_os\lab3
rm -r build

# Или в PowerShell:
Remove-Item -Recurse -Force build
```

## Возможные проблемы

### 1. CMake не найден

**Ошибка:** `'cmake' is not recognized`

**Решение:**
- Установите CMake: https://cmake.org/download/
- Добавьте CMake в PATH
- Или используйте "Developer Command Prompt for VS"

### 2. Компилятор не найден

**Ошибка:** `No CMAKE_CXX_COMPILER could be found`

**Решение:**
- Установите Visual Studio с C++ компонентами
- Или установите MinGW-w64
- Запустите CMake из "Developer Command Prompt for VS"

### 3. Ninja не найден

**Ошибка:** `'ninja' is not recognized`

**Решение:**
- Установите Ninja: `choco install ninja`
- Или используйте другой генератор: Visual Studio или MinGW

### 4. child.exe не запускается

**Ошибка:** `Не удалось создать дочерний процесс`

**Решение:**
- Убедитесь, что `child.exe` находится в той же директории, что и `parent.exe`
- Проверьте, что оба файла скомпилированы

### 5. Ошибка открытия file mapping

**Ошибка:** `Не удалось открыть file mapping`

**Решение:**
- Убедитесь, что parent запустился корректно
- Проверьте логи parent процесса
- Увеличьте Sleep() в parent.cpp после CreateProcess()

## Отладка

### Запуск с отладочной информацией:

```powershell
# Соберите в Debug режиме
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
ninja

# Или для Visual Studio:
cmake --build . --config Debug
```

### Просмотр логов child процесса:

Child процесс выводит логи в stderr, но они могут быть не видны. Для их просмотра можно перенаправить вывод:

```powershell
# Вручную запустите child.exe в отдельном окне, чтобы видеть его вывод
start cmd /k "child.exe"
```

Однако в нормальном режиме работы child запускается автоматически родителем.

## Дополнительные опции CMake

### Сборка с дополнительными предупреждениями:

```powershell
cmake .. -G Ninja -DCMAKE_CXX_FLAGS="/W4"
ninja
```

### Сборка в режиме Release с оптимизациями:

```powershell
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

### Вывод подробной информации при сборке:

```powershell
cmake .. -G Ninja
ninja -v  # verbose mode
```

## Тестирование

### Создайте свой тестовый файл:

```powershell
# Создайте файл с произвольными числами
echo 100 > my_test.txt
echo 101 >> my_test.txt
echo 102 >> my_test.txt
echo 103 >> my_test.txt  # 103 - простое, программа завершится

# Запустите программу
echo my_test.txt | .\parent.exe
```

### Тестирование с отрицательным числом:

```powershell
echo 4 > test_negative.txt
echo 6 >> test_negative.txt
echo -5 >> test_negative.txt

echo test_negative.txt | .\parent.exe
```

### Тестирование с простым числом:

```powershell
echo 9 > test_prime.txt
echo 15 >> test_prime.txt
echo 17 >> test_prime.txt  # 17 - простое, программа завершится

echo test_prime.txt | .\parent.exe
```

## Заключение

Используйте **Ninja** для быстрой сборки или **Visual Studio** для полноценной IDE с отладчиком.

Если возникли проблемы, проверьте:
1. ✅ CMake установлен и в PATH
2. ✅ Компилятор C++17 установлен
3. ✅ Директория build создана
4. ✅ child.exe и parent.exe в одной директории
5. ✅ test_numbers.txt скопирован в директорию с exe

