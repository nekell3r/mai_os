#include "sort_interface.h"

extern "C" {

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

