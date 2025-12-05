#include "sort_interface.h"
#include <algorithm>

extern "C" {

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

