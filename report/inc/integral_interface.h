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

// Contract: Calculate integral of sin(x) on [A, B] with step e
// Returns: calculated integral value
DLL_EXPORT float SinIntegral(float A, float B, float e);

#ifdef __cplusplus
}
#endif

