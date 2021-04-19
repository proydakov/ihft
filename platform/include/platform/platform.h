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
    static void set_current_thread_name(const char* name);

    // CPU isolation
    // https://lwn.net/Articles/816298/
    // https://www.suse.com/support/kb/doc/?id=000017747
    // https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux_for_real_time/7/html/tuning_guide/isolating_cpus_using_tuned-profiles-realtime

    // Change current thread cpu
    static void set_current_thread_cpu(unsigned cpu);

    // Check cpu isolation
    static bool get_cpu_isolation_status(unsigned cpu);
};

}
