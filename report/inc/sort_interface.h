#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
    #ifdef BUILD_DLL
        #define DLL_EXPORT __declspec(dllexport)
    #elif defined(USE_STATIC_LINK)
        #define DLL_EXPORT
    #else
        #define DLL_EXPORT __declspec(dllimport)
    #endif
#else
    #define DLL_EXPORT
#endif

// Contract: Sort integer array
// Returns: pointer to sorted array (same memory location)
DLL_EXPORT int* Sort(int* array, int size);

#ifdef __cplusplus
}
#endif

