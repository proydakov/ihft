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

    // Check hyper-threading
    static bool is_smt_active() noexcept;

    // Check transparent huge pages is [always] or [madvise]
    static bool is_transparent_huge_pages_active() noexcept;

    // Check swaps
    static bool is_swap_active() noexcept;
};

}
