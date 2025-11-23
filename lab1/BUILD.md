# Быстрая инструкция по сборке

## Сборка

```powershell
# Из директории lab1/
mkdir build
cd build

# Используем Ninja (автоопределение компилятора)
cmake .. -G Ninja
ninja

# Или Visual Studio (если установлена)
# cmake .. -G "Visual Studio 17 2022"
# cmake --build . --config Release
```

## Запуск

```powershell
# Из директории lab1/build/
cd src
copy ..\..\test_numbers.txt .
.\parent.exe
```

Введите: `test_numbers.txt`

## Структура (как в base/)

```
lab1/
├── common/          # Утилиты (логирование, ошибки)
│   ├── defines.h
│   ├── comm.h
│   ├── errors.h/hpp/cpp
│   └── CMakeLists.txt
├── src/             # Исполняемые файлы
│   ├── parent.cpp
│   ├── child.cpp
│   └── CMakeLists.txt
└── CMakeLists.txt   # Корневой файл
```

Подробности в [README.md](README.md)

