#include "../common/comm.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Static linking: both implementations are linked at compile time
// Rectangle method implementation
static float SinIntegralRectangles(float A, float B, float e) {
    if (A >= B || e <= 0.0f) {
        return 0.0f;
    }
    
    float result = 0.0f;
    float x = A;
    
    while (x < B) {
        float width = (x + e < B) ? e : (B - x);
        float mid_x = x + width / 2.0f;
        result += width * sinf(mid_x);
        x += width;
    }
    
    return result;
}

// Trapezoidal method implementation
static float SinIntegralTrapezoids(float A, float B, float e) {
    if (A >= B || e <= 0.0f) {
        return 0.0f;
    }
    
    float result = 0.0f;
    float x = A;
    
    while (x < B) {
        float width = (x + e < B) ? e : (B - x);
        float f_left = sinf(x);
        float f_right = sinf(x + width);
        result += width * (f_left + f_right) / 2.0f;
        x += width;
    }
    
    return result;
}

int main() {
    std::cout << "=== Program 1: Static Linking ===" << std::endl;
    std::cout << "Using: Both methods (statically linked)" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  1 <A> <B> <e>  - Calculate integral of sin(x) on [A, B] with step e (Rectangle method)" << std::endl;
    std::cout << "  2 <A> <B> <e>  - Calculate integral of sin(x) on [A, B] with step e (Trapezoidal method)" << std::endl;
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
        
        if (cmd == "1" || cmd == "2") {
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
            
            float result;
            std::string method;
            
            if (cmd == "1") {
                result = SinIntegralRectangles(A, B, e);
                method = "Rectangle";
            } else {
                result = SinIntegralTrapezoids(A, B, e);
                method = "Trapezoidal";
            }
            
            LogMsg("program1", "Integral of sin(x) on [" + std::to_string(A) + ", " + 
                   std::to_string(B) + "] with step " + std::to_string(e) + 
                   " (" + method + " method):");
            std::cout << "Result: " << std::fixed << std::setprecision(6) << result << std::endl;
        } else {
            LogErr("program1", "Unknown command. Use '1 <A> <B> <e>', '2 <A> <B> <e>' or 'exit'");
        }
    }
    
    LogMsg("program1", "Exiting...");
    return 0;
}
