#pragma once

#include <cstdint>

#if defined(__x86_64__)
#include <x86intrin.h>
#endif

#if defined(__aarch64__) && defined(__APPLE__)
#include <mach/mach_time.h>
#endif

// Portable fallbacks for architectures without a dedicated counter/pause hint.
#if !defined(__x86_64__) && !defined(__aarch64__)
#include <atomic>
#include <chrono>
#endif

namespace ihft::timer
{

inline std::uint64_t cpu_counter() noexcept
{
#if defined(__x86_64__)
    return __rdtsc();
#elif defined(__aarch64__)
    #if defined(__APPLE__)
    // Apple silicon does not expose the cycle counter through an intrinsic;
    // mach_absolute_time() is the documented monotonic tick source.
    return mach_absolute_time();
    #else
    // Read the virtual counter. The "memory" clobber prevents the compiler
    // from reordering surrounding code across the measurement point.
    std::uint64_t value{};
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(value) : : "memory");
    // Умножение на 100 это моя оценка в сравнении с x64_86.
    // Таймер на ARM тикает явно не так часто как на amd64.
    return value * 100;
    #endif
#else
    return static_cast<std::uint64_t>(
        std::chrono::steady_clock::now().time_since_epoch().count());
#endif
}

inline void cpu_pause() noexcept
{
#if defined(__x86_64__)
    _mm_pause();
#elif defined(__aarch64__) || defined(__arm__)
    __asm__ volatile("yield" ::: "memory");
#else
    std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
}

}
