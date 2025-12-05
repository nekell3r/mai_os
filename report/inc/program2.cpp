#include "../common/comm.h"
#include <windows.h>
#include <iomanip>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

// Function pointer types
typedef float (*IntegralFunc)(float, float, float);
typedef int* (*SortFunc)(int*, int);

class DynamicLibrary {
private:
    HMODULE hModule;
    IntegralFunc integralFunc;
    SortFunc sortFunc;
    std::string currentLib;
    
public:
    DynamicLibrary() : hModule(nullptr), integralFunc(nullptr), sortFunc(nullptr) {}
    
    ~DynamicLibrary() {
        unload();
    }
    
    bool load(const std::string& dllPath) {
        unload();
        
        hModule = LoadLibraryA(dllPath.c_str());
        if (!hModule) {
            DWORD error = GetLastError();
            LogErr("program2", "Failed to load library: " + dllPath + " (Error: " + std::to_string(error) + ")");
            return false;
        }
        
        // Get both functions from the library
        integralFunc = (IntegralFunc)GetProcAddress(hModule, "SinIntegral");
        if (!integralFunc) {
            LogErr("program2", "Failed to get SinIntegral function address");
            FreeLibrary(hModule);
            hModule = nullptr;
            return false;
        }
        
        sortFunc = (SortFunc)GetProcAddress(hModule, "Sort");
        if (!sortFunc) {
            LogErr("program2", "Failed to get Sort function address");
            FreeLibrary(hModule);
            hModule = nullptr;
            return false;
        }
        
        currentLib = dllPath;
        LogMsg("program2", "Library loaded: " + dllPath);
        return true;
    }
    
    void unload() {
        if (hModule) {
            FreeLibrary(hModule);
            hModule = nullptr;
            integralFunc = nullptr;
            sortFunc = nullptr;
            LogMsg("program2", "Library unloaded");
        }
    }
    
    bool isLoaded() const {
        return hModule != nullptr && integralFunc != nullptr && sortFunc != nullptr;
    }
    
    IntegralFunc getIntegralFunc() const {
        return integralFunc;
    }
    
    SortFunc getSortFunc() const {
        return sortFunc;
    }
    
    std::string getCurrentLib() const {
        return currentLib;
    }
    
    std::string getMethodName() const {
        if (currentLib == "rectangles.dll") {
            return "Rectangle method / Bubble Sort";
        } else if (currentLib == "trapezoids.dll") {
            return "Trapezoidal method / QuickSort";
        }
        return "Unknown";
    }
};

int main() {
    std::cout << "=== Program 2: Dynamic Loading ===" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  0  - Switch between rectangles.dll and trapezoids.dll" << std::endl;
    std::cout << "  1 <A> <B> <e>  - Calculate integral of sin(x) on [A, B] with step e" << std::endl;
    std::cout << "  2 <num1> <num2> ... <numN>  - Sort array" << std::endl;
    std::cout << "  exit  - Exit program" << std::endl;
    std::cout << std::endl;
    
    DynamicLibrary lib;
    bool useRectangles = true;
    
    // Load initial library
    if (!lib.load("rectangles.dll")) {
        LogErr("program2", "Failed to load initial library. Make sure DLLs are in the same directory.");
        return 1;
    }
    
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        
        if (line.empty()) continue;
        
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t");
        line = line.substr(start, end - start + 1);
        
        if (line.empty()) continue;
        
        if (line == "exit") {
            break;
        }
        
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        
        if (cmd.empty()) continue;
        
        if (cmd == "0") {
            // Switch between libraries
            useRectangles = !useRectangles;
            std::string dllName = useRectangles ? "rectangles.dll" : "trapezoids.dll";
            
            if (lib.load(dllName)) {
                LogMsg("program2", "Switched to: " + lib.getMethodName());
            }
            
        } else if (cmd == "1") {
            // Integral calculation
            if (!lib.isLoaded()) {
                LogErr("program2", "Library not loaded");
                continue;
            }
            
            float A, B, e;
            if (!(iss >> A >> B >> e)) {
                LogErr("program2", "Invalid arguments. Expected: <A> <B> <e>");
                continue;
            }
            
            if (A >= B) {
                LogErr("program2", "A must be less than B");
                continue;
            }
            
            if (e <= 0.0f) {
                LogErr("program2", "Step e must be positive");
                continue;
            }
            
            IntegralFunc func = lib.getIntegralFunc();
            float result = func(A, B, e);
            
            std::string method = useRectangles ? "Rectangle" : "Trapezoidal";
            LogMsg("program2", "Integral of sin(x) on [" + std::to_string(A) + ", " + 
                   std::to_string(B) + "] with step " + std::to_string(e) + 
                   " (" + method + " method):");
            std::cout << "Result: " << std::fixed << std::setprecision(6) << result << std::endl;
            
        } else if (cmd == "2") {
            // Array sorting
            if (!lib.isLoaded()) {
                LogErr("program2", "Library not loaded");
                continue;
            }
            
            std::vector<int> arr;
            int num;
            while (iss >> num) {
                arr.push_back(num);
            }
            
            if (arr.empty()) {
                LogErr("program2", "No numbers provided");
                continue;
            }
            
            if (arr.size() > 1000) {
                LogErr("program2", "Array too large (max 1000 elements)");
                continue;
            }
            
            int size = static_cast<int>(arr.size());
            
            std::string method = useRectangles ? "Bubble Sort" : "QuickSort (Hoare)";
            LogMsg("program2", "Input array (" + method + "):");
            for (int i = 0; i < size; ++i) {
                std::cout << arr[i] << " ";
            }
            std::cout << std::endl;
            
            SortFunc func = lib.getSortFunc();
            int* result = func(arr.data(), size);
            
            LogMsg("program2", "Sorted array:");
            for (int i = 0; i < size; ++i) {
                std::cout << result[i] << " ";
            }
            std::cout << std::endl;
            
        } else {
            LogErr("program2", "Unknown command. Use '0', '1 <A> <B> <e>', '2 <num1> <num2> ... <numN>' or 'exit'");
        }
    }
    
    LogMsg("program2", "Exiting...");
    return 0;
}
