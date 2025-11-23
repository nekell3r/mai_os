#include "median_filter.h"
#include "../common/comm.h"

void printUsage(const char* program_name) {
    std::cout << "Использование: " << program_name << " [опции]\n\n";
    std::cout << "Опции:\n";
    std::cout << "  -f <файл>        Входной файл с матрицей\n";
    std::cout << "  -g <rows> <cols> Сгенерировать случайную матрицу размером rows x cols\n";
    std::cout << "  -w <размер>      Размер окна фильтра (по умолчанию 3)\n";
    std::cout << "  -k <число>       Количество итераций фильтра (по умолчанию 1)\n";
    std::cout << "  -t <число>       Максимальное количество потоков (по умолчанию 1)\n";
    std::cout << "  -o <файл>        Выходной файл для сохранения результата\n";
    std::cout << "  -p               Вывести результат на экран (только для маленьких матриц)\n";
    std::cout << "  -h               Показать эту справку\n\n";
    std::cout << "Пример:\n";
    std::cout << "  " << program_name << " -f input.txt -w 5 -k 3 -t 4 -o output.txt\n";
    std::cout << "  " << program_name << " -g 100 100 -w 3 -k 2 -t 8\n";
}

int main(int argc, char* argv[]) {
    std::cout << "==============================================\n";
    std::cout << "  Медианный фильтр с многопоточной обработкой\n";
    std::cout << "  Windows версия\n";
    std::cout << "==============================================\n\n";
    
    // Параметры по умолчанию
    std::string input_file;
    std::string output_file;
    bool generate_random = false;
    size_t gen_rows = 100;
    size_t gen_cols = 100;
    bool print_result = false;
    
    MedianFilterParams params;
    params.window_size = 3;
    params.iterations = 1;
    params.max_threads = 1;
    
    // Парсинг аргументов командной строки
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-f" && i + 1 < argc) {
            input_file = argv[++i];
        } else if (arg == "-g" && i + 2 < argc) {
            generate_random = true;
            gen_rows = std::stoi(argv[++i]);
            gen_cols = std::stoi(argv[++i]);
        } else if (arg == "-w" && i + 1 < argc) {
            params.window_size = std::stoi(argv[++i]);
            if (params.window_size % 2 == 0) {
                LogErr("main", "Размер окна должен быть нечетным");
                return 1;
            }
        } else if (arg == "-k" && i + 1 < argc) {
            params.iterations = std::stoi(argv[++i]);
        } else if (arg == "-t" && i + 1 < argc) {
            params.max_threads = std::stoi(argv[++i]);
            if (params.max_threads < 1) {
                LogErr("main", "Количество потоков должно быть >= 1");
                return 1;
            }
        } else if (arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        } else if (arg == "-p") {
            print_result = true;
        } else {
            std::cerr << "Неизвестная опция: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Проверка входных данных
    if (!generate_random && input_file.empty()) {
        LogErr("main", "Необходимо указать входной файл (-f) или сгенерировать матрицу (-g)");
        std::cout << "\n";
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        // Загрузка или генерация матрицы
        Matrix input_matrix(0, 0);
        
        if (generate_random) {
            std::stringstream msg;
            msg << "Генерация случайной матрицы " << gen_rows << "x" << gen_cols;
            LogMsg("main", msg.str());
            input_matrix = Matrix::generateRandom(gen_rows, gen_cols, 0, 255);
        } else {
            std::stringstream msg;
            msg << "Загрузка матрицы из файла: " << input_file;
            LogMsg("main", msg.str());
            input_matrix = Matrix::loadFromFile(input_file);
        }
        
        std::stringstream size_msg;
        size_msg << "Размер матрицы: " << input_matrix.rows() << "x" << input_matrix.cols();
        LogMsg("main", size_msg.str());
        
        // Вывод исходной матрицы (если запрошено и матрица небольшая)
        if (print_result && input_matrix.rows() <= 10 && input_matrix.cols() <= 10) {
            std::cout << "\nИсходная матрица:\n";
            input_matrix.print();
        }
        
        // Настройка фильтра
        std::cout << "\nПараметры фильтра:\n";
        std::cout << "  Размер окна: " << params.window_size << "x" << params.window_size << "\n";
        std::cout << "  Количество итераций: " << params.iterations << "\n";
        std::cout << "  Максимальное количество потоков: " << params.max_threads << "\n\n";
        
        // Применение фильтра
        LogMsg("main", "Начало применения медианного фильтра");
        MedianFilter filter(params);
        Matrix output_matrix = filter.apply(input_matrix);
        
        // Вывод статистики
        auto stats = filter.getStats();
        std::cout << "\n==============================================\n";
        std::cout << "Статистика выполнения:\n";
        std::cout << "  Общее время: " << stats.total_time_ms << " мс\n";
        std::cout << "  Среднее время итерации: " << stats.avg_iteration_time_ms << " мс\n";
        std::cout << "  Использовано потоков: " << stats.threads_used << "\n";
        std::cout << "==============================================\n\n";
        
        // Вывод результата (если запрошено и матрица небольшая)
        if (print_result && output_matrix.rows() <= 10 && output_matrix.cols() <= 10) {
            std::cout << "Результирующая матрица:\n";
            output_matrix.print();
        }
        
        // Сохранение результата
        if (!output_file.empty()) {
            std::stringstream save_msg;
            save_msg << "Сохранение результата в файл: " << output_file;
            LogMsg("main", save_msg.str());
            output_matrix.saveToFile(output_file);
        }
        
        LogMsg("main", "Готово!");
        
        std::cout << "\nДля просмотра потоков во время выполнения используйте:\n";
        std::cout << "  Task Manager -> Подробности -> Выберите процесс -> Потоки\n";
        std::cout << "  Или PowerShell: Get-Process -Name median_filter | Select-Object -ExpandProperty Threads\n";
        
    } catch (const std::exception& e) {
        std::stringstream err_msg;
        err_msg << "Исключение: " << e.what();
        LogErr("main", err_msg.str());
        return 1;
    }
    
    return 0;
}
