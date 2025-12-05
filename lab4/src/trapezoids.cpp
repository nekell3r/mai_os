#include "integral_interface.h"
#include <cmath>

extern "C" {

// Trapezoidal method implementation
float SinIntegral(float A, float B, float e) {
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

}

