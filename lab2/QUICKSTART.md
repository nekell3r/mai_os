# Быстрый старт

## Автоматическая сборка (рекомендуется)

```powershell
# Из директории lab2/
.\rebuild.ps1

# Для полной пересборки
.\rebuild.ps1 -Clean
```

## Ручная сборка с Ninja

```powershell
# 1. Перейти в директорию lab2
cd lab2

# 2. Создать директорию для сборки
mkdir build
cd build

# 3. Сгенерировать файлы сборки
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..

# 4. Собрать
ninja

# 5. Запустить тест
cd src
.\median_filter.exe -f ..\..\test_matrix.txt -w 3 -k 1 -t 4 -p
```

## Быстрый тест производительности

```powershell
# Из директории lab2/build/src/
.\median_filter.exe -g 1000 1000 -w 5 -k 3 -t 1   # 1 поток
.\median_filter.exe -g 1000 1000 -w 5 -k 3 -t 4   # 4 потока
.\median_filter.exe -g 1000 1000 -w 5 -k 3 -t 8   # 8 потоков
```

## Автоматическое тестирование

```powershell
# Из директории lab2/
copy test_performance.ps1 build\
cd build
.\test_performance.ps1
```

## Если Ninja не установлен

### Установка через Chocolatey
```powershell
choco install ninja
```

### Установка через Scoop
```powershell
scoop install ninja
```

### Проверка
```powershell
ninja --version
cmake --version
```

## Пересборка после изменений

### Вариант 1: Автоматический скрипт
```powershell
# Из директории lab2/
.\rebuild.ps1
```

### Вариант 2: Вручную
```powershell
# Из директории lab2/build/
ninja
```

## Полная очистка

### Вариант 1: Автоматический скрипт
```powershell
# Из директории lab2/
.\rebuild.ps1 -Clean
```

### Вариант 2: Вручную
```powershell
# Из директории lab2/
rmdir /s /q build
mkdir build
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

## Типичные проблемы

### "ninja: command not found"
Установите Ninja (см. выше).

### "MSVC not found" или ошибки компилятора
Запустите PowerShell из **Developer PowerShell for VS** или **Developer Command Prompt for VS**.

### "median_filter.exe не найден"
Убедитесь, что сборка прошла успешно без ошибок. Исполняемый файл должен быть в `build/src/median_filter.exe`.

