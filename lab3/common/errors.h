#pragma once

#include "defines.h"

#define ASSERT_MSG(condition, message)                          \
    if (!(condition)) {                                         \
        std::cerr << "ASSERTION FAILED: " << #condition << "\n" \
                  << "Message: " << message << "\n"             \
                  << "File: " << __FILE__ << "\n"               \
                  << "Line: " << __LINE__ << "\n";              \
        AssertBreak();                                          \
    }

void AssertBreak();

