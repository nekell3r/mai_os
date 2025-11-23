#include "errors.h"
#include <cstdlib>

void err::DebugBreak()
{
    exit(1);  // ✅ Завершает программу и вызывает деструкторы
              // (закрываются pipe'ы и другие ресурсы)
}

