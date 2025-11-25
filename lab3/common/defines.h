#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define OS_WINDOWS 1
#define OS_LINUX 0

#include <windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#elif defined(__linux__)
#define OS_WINDOWS 0
#define OS_LINUX 1

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <sstream>

#else
#error "Unsupported platform"
#endif

