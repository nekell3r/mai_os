#include "errors.h"

void err::DebugBreak()
{
    int* pointer = nullptr;
    [[maybe_unused]] int val = *pointer;
}