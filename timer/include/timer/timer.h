#pragma once

#include <cstdint>

#if __x86_64__
#include <x86intrin.h>
#endif

namespace ihft::timer
{

inline std::uint64_t cpu_counter()
{
#if __x86_64__
    // @todo : support ARM64
    return __rdtsc();
#else
    return 0;
#endif
}

inline void cpu_pause()
{
#if __x86_64__
    _mm_pause();
#else
    asm volatile("yield");
#endif
}

}
