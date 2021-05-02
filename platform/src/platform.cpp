#include <platform/platform.h>
#include <platform/private/isolation.h>

#include <fstream>
#include <filesystem>

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

    bool platform::is_transparent_huge_pages_active() noexcept
    {
        std::ifstream file("/sys/kernel/mm/transparent_hugepage/enabled");

        bool active = true;
        std::string value;
        while(file >> value)
        {
            if ("[never]" == value)
            {
                active = false;
            }
        }

        return active;
    }

    bool platform::is_swap_active() noexcept
    {
        std::ifstream file("/proc/swaps");

        size_t lines = 0;
        std::string buffer;
        while(std::getline(file, buffer))
        {
            lines++;
        }

        return lines != 1;
    }

    bool platform::is_scaling_governor_use_performance_mode() noexcept
    {
        size_t total_cpus = 0;
        size_t total_performance = 0;
        for(auto const& p : std::filesystem::recursive_directory_iterator("/sys/devices/system/cpu/cpufreq"))
        {
            std::string const str = p.path();
            if (str.ends_with("/scaling_governor"))
            {
                total_cpus++;
                std::ifstream file(str);
                std::string buffer;
                file >> buffer;
                if ("performance" == buffer)
                {
                    total_performance++;
                }
            }
        }
        return total_cpus > 0 && total_cpus == total_performance;
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

    bool platform::is_transparent_huge_pages_active() noexcept
    {
        return true;
    }

    bool platform::is_swap_active() noexcept
    {
        return true;
    }

    bool platform::is_scaling_governor_use_performance_mode() noexcept
    {
        return false;
    }
}

#endif
