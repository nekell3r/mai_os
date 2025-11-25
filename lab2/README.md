# Лабораторная работа №2 - Медианный фильтр (многопоточность)

**Вариант 11:** K раз наложить медианный фильтр на матрицу целых чисел.

---

## Сборка

```powershell
# Автоматически
.\rebuild.ps1

# Или вручную
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

---

## Запуск

```bash
cd build\src
.\median_filter.exe [опции]
```

### Опции

```
-f <файл>       Входной файл
-g <rows> <cols>  Сгенерировать матрицу
-w <размер>     Размер окна (по умолч. 3)
-k <число>      Итерации (по умолч. 1)
-t <число>      Потоки (по умолч. 1)
-o <файл>       Выходной файл
-p              Вывести результат на экран
-h              Справка
```

### Примеры

```bash
# Обработка файла с 4 потоками
.\median_filter.exe -f ..\..\test_matrix.txt -w 3 -k 1 -t 4 -p

# 1000x1000 матрица, разные потоки
.\median_filter.exe -g 1000 1000 -w 5 -k 3 -t 1
.\median_filter.exe -g 1000 1000 -w 5 -k 3 -t 4
.\median_filter.exe -g 1000 1000 -w 5 -k 3 -t 8
```

---

## Тестирование производительности

```powershell
# Автоматический тест
copy test_performance.ps1 build\
cd build
.\test_performance.ps1
```

Результаты → `build/src/performance_results.csv`

---

## Трассировка (WinDbg)

```powershell
windbg .\build\src\median_filter.exe -g 100 100 -w 3 -k 1 -t 2
```

В WinDbg:
```
.logopen D:\programming_projects\mai_os\lab1\build\src\trace.log
bp ntdll!NtCreateThreadEx ".echo === NtCreateThreadEx ===; g"
bp ntdll!NtWaitForMultipleObjects ".echo === NtWaitForMultipleObjects ===; g"
bp ntdll!NtClose ".echo NtClose; g"
bp ntdll!NtWriteFile ".echo NtWriteFile; g"
g
.logclose
```
