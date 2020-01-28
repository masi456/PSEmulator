#include "platform.hpp"

#ifdef _MSC_VER
#include <intrin.h>
void platform::debuggerBreak() {
    __debugbreak();
}
#endif

