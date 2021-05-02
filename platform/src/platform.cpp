#include <platform/platform.h>
#include <platform/private/isolation.h>

#include <fstream>

#ifdef __linux__

#include <sched.h>
#include <sys/prctl.h>

namespace
{
    static const ihft::impl::isolation g_isolation("/proc/cmdline");
}

namespace ihft
{
    void platform::set_current_thread_name(const char* name) noexcept
    {
        prctl(PR_SET_NAME, name, 0, 0, 0);
    }

    void platform::set_current_thread_cpu(unsigned cpu) noexcept
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(static_cast<unsigned long>(cpu), &cpuset);

        sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
    }

    bool platform::get_cpu_isolation_status(unsigned cpu) noexcept
    {
        return g_isolation.is_isolated(cpu);
    }

    bool platform::is_smt_active() noexcept
    {
        std::ifstream file("/sys/devices/system/cpu/smt/active");
        std::string value;
        file >> value;

        return value != "0";
    }
}

#else

namespace ihft
{
    void platform::set_current_thread_name(const char*) noexcept
    {
    }

    void platform::set_current_thread_cpu(unsigned) noexcept
    {
    }

    bool platform::get_cpu_isolation_status(unsigned) noexcept
    {
        return false;
    }

    bool platform::is_smt_active() noexcept
    {
        return true;
    }
}

#endif
