#include "../common/comm.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

// Function pointer type matching Sort signature
typedef int* (*SortFunc)(int*, int);

class DynamicLibrary {
private:
    HMODULE hModule;
    SortFunc sortFunc;
    std::string currentLib;
    
public:
    DynamicLibrary() : hModule(nullptr), sortFunc(nullptr) {}
    
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
            sortFunc = nullptr;
            LogMsg("program2", "Library unloaded");
        }
    }
    
    bool isLoaded() const {
        return hModule != nullptr && sortFunc != nullptr;
    }
    
    SortFunc getSortFunc() const {
        return sortFunc;
    }
    
    std::string getCurrentLib() const {
        return currentLib;
    }
};

int main() {
    std::cout << "=== Program 2: Dynamic Loading ===" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  0  - Switch between bubble_sort.dll and quicksort.dll" << std::endl;
    std::cout << "  1 <num1> <num2> ... <numN>  - Sort array using Bubble Sort" << std::endl;
    std::cout << "  2 <num1> <num2> ... <numN>  - Sort array using QuickSort (Hoare)" << std::endl;
    std::cout << "  exit  - Exit program" << std::endl;
    std::cout << std::endl;
    
    DynamicLibrary lib;
    bool useBubbleSort = true;
    
    // Load initial library
    if (!lib.load("bubble_sort.dll")) {
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
            useBubbleSort = !useBubbleSort;
            std::string dllName = useBubbleSort ? "bubble_sort.dll" : "quicksort.dll";
            
            if (lib.load(dllName)) {
                std::string method = useBubbleSort ? "Bubble Sort" : "QuickSort (Hoare)";
                LogMsg("program2", "Switched to: " + method);
            }
        } else if (cmd == "1" || cmd == "2") {
            // Command "1" uses Bubble Sort, command "2" uses QuickSort
            bool needBubbleSort = (cmd == "1");
            
            // Load appropriate DLL if needed
            if (needBubbleSort && lib.getCurrentLib() != "bubble_sort.dll") {
                if (!lib.load("bubble_sort.dll")) {
                    LogErr("program2", "Failed to load bubble_sort.dll");
                    continue;
                }
            } else if (!needBubbleSort && lib.getCurrentLib() != "quicksort.dll") {
                if (!lib.load("quicksort.dll")) {
                    LogErr("program2", "Failed to load quicksort.dll");
                    continue;
                }
            }
            
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
            
            std::string method = needBubbleSort ? "Bubble Sort" : "QuickSort (Hoare)";
            LogMsg("program2", "Input array (" + method + "):");
            for (int i = 0; i < size; ++i) {
                std::cout << arr[i] << " ";
            }
            std::cout << std::endl;
            
            SortFunc sortFunc = lib.getSortFunc();
            int* result = sortFunc(arr.data(), size);
            
            LogMsg("program2", "Sorted array:");
            for (int i = 0; i < size; ++i) {
                std::cout << result[i] << " ";
            }
            std::cout << std::endl;
        } else {
            LogErr("program2", "Unknown command. Use '0', '1 <num1> <num2> ... <numN>', '2 <num1> <num2> ... <numN>' or 'exit'");
        }
    }
    
    LogMsg("program2", "Exiting...");
    return 0;
}

