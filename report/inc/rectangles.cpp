#include "integral_interface.h"
#include "sort_interface.h"
#include <cmath>

extern "C" {

// Rectangle method implementation (integral)
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

// Bubble sort implementation
int* Sort(int* array, int size) {
    if (!array || size <= 0) {
        return array;
    }
    
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
    
    return array;
}

}
