#include "integral_interface.h"
#include "sort_interface.h"
#include <cmath>

extern "C" {

// Trapezoidal method implementation (integral)
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

// Helper function for QuickSort (Hoare partition)
static int partition(int* array, int low, int high) {
    int pivot = array[(low + high) / 2];
    int i = low - 1;
    int j = high + 1;
    
    while (true) {
        do {
            i++;
        } while (array[i] < pivot);
        
        do {
            j--;
        } while (array[j] > pivot);
        
        if (i >= j) {
            return j;
        }
        
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// Helper function for QuickSort (recursive)
static void quicksort_recursive(int* array, int low, int high) {
    if (low < high) {
        int pi = partition(array, low, high);
        quicksort_recursive(array, low, pi);
        quicksort_recursive(array, pi + 1, high);
    }
}

// QuickSort (Hoare) implementation
int* Sort(int* array, int size) {
    if (!array || size <= 0) {
        return array;
    }
    
    quicksort_recursive(array, 0, size - 1);
    return array;
}

}
