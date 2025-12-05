#include "../common/comm.h"
#include "integral_interface.h"
#include "sort_interface.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Static linking: uses rectangles implementation (both functions statically linked)
// This includes: SinIntegral (Rectangle method) and Sort (Bubble Sort)
// rectangles.cpp is linked statically in CMakeLists.txt

int main() {
    std::cout << "=== Program 1: Static Linking ===" << std::endl;
    std::cout << "Using: rectangles implementation (statically linked)" << std::endl;
    std::cout << "  - SinIntegral: Rectangle method" << std::endl;
    std::cout << "  - Sort: Bubble Sort" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  1 <A> <B> <e>  - Calculate integral of sin(x) on [A, B] with step e" << std::endl;
    std::cout << "  2 <num1> <num2> ... <numN>  - Sort array" << std::endl;
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
            // Integral calculation
            float A, B, e;
            if (!(iss >> A >> B >> e)) {
                LogErr("program1", "Invalid arguments. Expected: <A> <B> <e>");
                continue;
            }
            
            if (A >= B) {
                LogErr("program1", "A must be less than B");
                continue;
            }
            
            if (e <= 0.0f) {
                LogErr("program1", "Step e must be positive");
                continue;
            }
            
            float result = SinIntegral(A, B, e);
            
            LogMsg("program1", "Integral of sin(x) on [" + std::to_string(A) + ", " + 
                   std::to_string(B) + "] with step " + std::to_string(e) + 
                   " (Rectangle method):");
            std::cout << "Result: " << std::fixed << std::setprecision(6) << result << std::endl;
            
        } else if (cmd == "2") {
            // Array sorting
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
            
            LogMsg("program1", "Input array (Bubble Sort):");
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
            LogErr("program1", "Unknown command. Use '1 <A> <B> <e>', '2 <num1> <num2> ... <numN>' or 'exit'");
        }
    }
    
    LogMsg("program1", "Exiting...");
    return 0;
}
