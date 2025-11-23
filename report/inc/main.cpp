#include "median_filter.h"
#include "../common/comm.h"

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -f <file>        Input file with matrix\n";
    std::cout << "  -g <rows> <cols> Generate random matrix\n";
    std::cout << "  -w <size>        Filter window size (default 3)\n";
    std::cout << "  -k <number>      Number of iterations (default 1)\n";
    std::cout << "  -t <number>      Maximum number of threads (default 1)\n";
    std::cout << "  -o <file>        Output file\n";
    std::cout << "  -p               Print result to screen\n";
    std::cout << "  -h               Show this help\n\n";
    std::cout << "Example:\n";
    std::cout << "  " << program_name << " -f input.txt -w 5 -k 3 -t 4 -o output.txt\n";
    std::cout << "  " << program_name << " -g 100 100 -w 3 -k 2 -t 8\n";
}

int main(int argc, char* argv[]) {
    std::cout << "==============================================\n";
    std::cout << "  Median Filter with Multithreading\n";
    std::cout << "  Windows version\n";
    std::cout << "==============================================\n\n";
    
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
                LogErr("main", "Window size must be odd");
                return 1;
            }
        } else if (arg == "-k" && i + 1 < argc) {
            params.iterations = std::stoi(argv[++i]);
        } else if (arg == "-t" && i + 1 < argc) {
            params.max_threads = std::stoi(argv[++i]);
            if (params.max_threads < 1) {
                LogErr("main", "Number of threads must be >= 1");
                return 1;
            }
        } else if (arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        } else if (arg == "-p") {
            print_result = true;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (!generate_random && input_file.empty()) {
        LogErr("main", "Must specify input file (-f) or generate matrix (-g)");
        std::cout << "\n";
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        Matrix input_matrix(0, 0);
        
        if (generate_random) {
            std::stringstream msg;
            msg << "Generating random matrix " << gen_rows << "x" << gen_cols;
            LogMsg("main", msg.str());
            input_matrix = Matrix::generateRandom(gen_rows, gen_cols, 0, 255);
        } else {
            std::stringstream msg;
            msg << "Loading matrix from file: " << input_file;
            LogMsg("main", msg.str());
            input_matrix = Matrix::loadFromFile(input_file);
        }
        
        std::stringstream size_msg;
        size_msg << "Matrix size: " << input_matrix.rows() << "x" << input_matrix.cols();
        LogMsg("main", size_msg.str());
        
        if (print_result && input_matrix.rows() <= 10 && input_matrix.cols() <= 10) {
            std::cout << "\nInput matrix:\n";
            input_matrix.print();
        }
        
        std::cout << "\nFilter parameters:\n";
        std::cout << "  Window size: " << params.window_size << "x" << params.window_size << "\n";
        std::cout << "  Iterations: " << params.iterations << "\n";
        std::cout << "  Max threads: " << params.max_threads << "\n\n";
        
        LogMsg("main", "Starting median filter");
        MedianFilter filter(params);
        Matrix output_matrix = filter.apply(input_matrix);
        
        auto stats = filter.getStats();
        std::cout << "\n==============================================\n";
        std::cout << "Execution Statistics:\n";
        std::cout << "  Total time: " << stats.total_time_ms << " ms\n";
        std::cout << "  Average iteration time: " << stats.avg_iteration_time_ms << " ms\n";
        std::cout << "  Threads used: " << stats.threads_used << "\n";
        std::cout << "==============================================\n\n";
        
        if (print_result && output_matrix.rows() <= 10 && output_matrix.cols() <= 10) {
            std::cout << "Output matrix:\n";
            output_matrix.print();
        }
        
        if (!output_file.empty()) {
            std::stringstream save_msg;
            save_msg << "Saving result to file: " << output_file;
            LogMsg("main", save_msg.str());
            output_matrix.saveToFile(output_file);
        }
        
        LogMsg("main", "Done");
        
        std::cout << "\nTo monitor threads during execution use:\n";
        std::cout << "  Task Manager -> Details -> Select process -> Threads\n";
        std::cout << "  Or PowerShell: Get-Process -Name median_filter | Select-Object -ExpandProperty Threads\n";
        
    } catch (const std::exception& e) {
        std::stringstream err_msg;
        err_msg << "Exception: " << e.what();
        LogErr("main", err_msg.str());
        return 1;
    }
    
    return 0;
}
