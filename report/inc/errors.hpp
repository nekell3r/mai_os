#pragma once

#include "defines.h"
#include <sstream>

template <typename... Args>
std::string Format(Args... args) {
    std::stringstream ss;
    (ss << ... << args);
    return ss.str();
}

