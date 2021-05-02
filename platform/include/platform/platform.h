#pragma once

namespace ihft
{

class platform
{
public:
    // Change current thread name. Could be useful with htop
    // The name can be up to 16 bytes long, including the terminating null byte.
    // (If the length of the string, including the terminating null byte,
    // exceeds 16 bytes, the string is silently truncated.)
    static void set_current_thread_name(const char* name) noexcept;

    // CPU isolation
    // https://lwn.net/Articles/816298/
    // https://www.suse.com/support/kb/doc/?id=000017747
    // https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux_for_real_time/7/html/tuning_guide/isolating_cpus_using_tuned-profiles-realtime

    // Change current thread cpu
    static void set_current_thread_cpu(unsigned cpu) noexcept;

    // Check cpu isolation
    static bool get_cpu_isolation_status(unsigned cpu) noexcept;

    //
    // Hugepages detector
    // https://rigtorp.se/hugepages/
    // Get total 1gb hugepages
    static unsigned total_1gb_hugepages() noexcept;

    // Get total 2mb hugepages
    static unsigned total_2mb_hugepages() noexcept;

    //
    // System cpu & memory features
    // https://rigtorp.se/low-latency-guide/
    //

    // All parameters below should be true for low latency

    // Check CPU frequency scaling governor mode
    static bool is_scaling_governor_use_performance_mode() noexcept;

    // All parameters below should be false for low latency

    // Check hyper-threading
    static bool is_smt_active() noexcept;

    // Check swaps
    static bool is_swap_active() noexcept;

    // Check transparent hugepages is [always] or [madvise]
    static bool is_transparent_hugepages_active() noexcept;
};

}
