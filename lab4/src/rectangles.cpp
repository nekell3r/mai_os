#include "integral_interface.h"
#include <cmath>

extern "C" {

// Rectangle method implementation
float SinIntegral(float A, float B, float e) {
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

}

