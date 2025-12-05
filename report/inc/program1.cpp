#include "../common/comm.h"
#include "sort_interface.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

// Static linking: library is linked at compile time
// This program uses bubble_sort implementation (linked statically)

int main() {
    std::cout << "=== Program 1: Static Linking ===" << std::endl;
    std::cout << "Using: Bubble Sort (statically linked)" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  1 <num1> <num2> ... <numN>  - Sort array" << std::endl;
    std::cout << "  exit  - Exit program" << std::endl;
    std::cout << std::endl;
    
    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        
        if (line.empty()) continue;
        
        if (line == "exit") {
            break;
        }
        
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "1") {
            std::vector<int> arr;
            int num;
            while (iss >> num) {
                arr.push_back(num);
            }
            
            if (arr.empty()) {
                LogErr("program1", "No numbers provided");
                continue;
            }
            
            if (arr.size() > 1000) {
                LogErr("program1", "Array too large (max 1000 elements)");
                continue;
            }
            
            int size = static_cast<int>(arr.size());
            
            LogMsg("program1", "Input array:");
            for (int i = 0; i < size; ++i) {
                std::cout << arr[i] << " ";
            }
            std::cout << std::endl;
            
            int* result = Sort(arr.data(), size);
            
            LogMsg("program1", "Sorted array:");
            for (int i = 0; i < size; ++i) {
                std::cout << result[i] << " ";
            }
            std::cout << std::endl;
        } else {
            LogErr("program1", "Unknown command. Use '1 <num1> <num2> ... <numN>' or 'exit'");
        }
    }
    
    LogMsg("program1", "Exiting...");
    return 0;
}

