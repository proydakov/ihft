#pragma once

//
// This code contains a lot of common methods for detecting system
// configuration or change some runtime parameters
//
// See examples: get_platform_info & set_thread_cpu & set_thread_name
//
namespace ihft::platform
{

struct trait
{
    // Change current thread name. Could be useful with htop
    // The name can be up to 16 bytes long, including the terminating null byte.
    // (If the length of the string, including the terminating null byte,
    // exceeds 16 bytes, the string is silently truncated.)
    static bool set_current_thread_name(const char* name) noexcept;

    // What CPU isolation is?
    // https://lwn.net/Articles/816298/
    // https://www.suse.com/support/kb/doc/?id=000017747
    // https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux_for_real_time/7/html/tuning_guide/isolating_cpus_using_tuned-profiles-realtime

    // Change current thread cpu
    static bool set_current_thread_cpu(unsigned cpu) noexcept;

    // Reset current thread cpu mask
    static bool reset_current_thread_cpu() noexcept;

    // Get total cpus count on current machine
    static unsigned get_total_cpus() noexcept;

    // Get current thread native id
    static long get_thread_id() noexcept;

    // This call locks all pages mapped into the address space of the calling process.
    // This includes the pages of the code, data, and stack segment,
    // as well as shared libraries, user space kernel data, shared memory, and memory-mapped files.
    // All mapped pages are guaranteed to be resident in RAM when the call returns successfully.
    // The pages are guaranteed to stay in RAM until later unlocked.
    // https://man7.org/linux/man-pages/man2/mlock.2.html
    static bool lock_memory_pages(bool current, bool future) noexcept;

    // Check cpu isolation
    static bool get_cpu_isolation_status(unsigned cpu) noexcept;

    // Check cpu nohz_full
    static bool get_cpu_nohz_full_status(unsigned cpu) noexcept;

    // Check cpu rcu_nocbs
    static bool get_cpu_rcu_nocbs_status(unsigned cpu) noexcept;

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
    static bool is_hyper_threading_active() noexcept;

    // Check swaps
    static bool is_swap_active() noexcept;

    // Check transparent hugepages is [always] or [madvise]
    static bool is_transparent_hugepages_active() noexcept;
};

}
