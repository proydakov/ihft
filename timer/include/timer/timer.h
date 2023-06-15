#pragma once

#include <x86intrin.h>

namespace ihft::timer
{

inline unsigned long long cpu_counter()
{
    // @todo : support ARM64
    return __rdtsc();
}

inline void cpu_pause()
{
    _mm_pause();
}

}
