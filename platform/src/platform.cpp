#include <platform/platform.h>

#ifdef __linux__

#include <platform/private/cmdline.h>

#include <cstring>
#include <fstream>
#include <charconv>
#include <filesystem>
#include <string_view>

#include <sched.h>
#include <sys/mman.h>
#include <sys/prctl.h>

namespace
{
    static const ihft::impl::cmdline g_cmdline("/proc/cmdline");

    cpu_set_t load_default_cpuset() noexcept
    {
        cpu_set_t set;
        sched_getaffinity(0, sizeof(cpu_set_t), &set);
        return set;
    }

    cpu_set_t const g_default_cpuset = load_default_cpuset();

    std::pair<unsigned, unsigned> get_hp_info_impl() noexcept
    {
        std::ifstream file("/proc/meminfo");

        unsigned total{};
        unsigned hpsize{};

        constexpr std::string_view hugepagesizeMark = "Hugepagesize:";
        constexpr std::string_view hugePagesTotalMark = "HugePages_Total:";

        std::string buffer;
        while(std::getline(file, buffer))
        {
            if (buffer.starts_with(hugepagesizeMark))
            {
                std::string_view view(buffer);
                view.remove_prefix(hugepagesizeMark.length());
                view.remove_prefix(std::min(view.find_first_not_of(' '), view.size()));
                std::from_chars(view.data(), view.data() + view.size(), hpsize);
            }
            else if (buffer.starts_with(hugePagesTotalMark))
            {
                std::string_view view(buffer);
                view.remove_prefix(hugePagesTotalMark.length());
                view.remove_prefix(std::min(view.find_first_not_of(' '), view.size()));
                std::from_chars(view.data(), view.data() + view.size(), total);
            }
        }

        return {total, hpsize};
    }
}

namespace ihft::platform
{
    bool set_current_thread_name(const char* name) noexcept
    {
        return 0 == prctl(PR_SET_NAME, name, 0, 0, 0);
    }

    bool set_current_thread_cpu(unsigned cpu) noexcept
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(static_cast<unsigned long>(cpu), &cpuset);

        return 0 == sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
    }

    bool reset_current_thread_cpu() noexcept
    {
        return 0 == sched_setaffinity(0, sizeof(cpu_set_t), &g_default_cpuset);
    }

    bool lock_memory_pages(bool current, bool future) noexcept
    {
        int flags = 0;

        if (current)
            flags |= MCL_CURRENT;
        if (future)
            flags |= MCL_FUTURE;

        return 0 == mlockall(flags);
    }

    bool get_cpu_isolation_status(unsigned cpu) noexcept
    {
        return g_cmdline.is_isolated(cpu);
    }

    bool get_cpu_nohz_full_status(unsigned cpu) noexcept
    {
        return g_cmdline.is_nohz_fulled(cpu);
    }

    bool get_cpu_rcu_nocbs_status(unsigned cpu) noexcept
    {
        return g_cmdline.is_rcu_nocbsed(cpu);
    }

    unsigned total_1gb_hugepages() noexcept
    {
        auto const [total, hpsize] = get_hp_info_impl();

        return 1048576u == hpsize ? total : 0u;
    }

    unsigned total_2mb_hugepages() noexcept
    {
        auto const [total, hpsize] = get_hp_info_impl();

        return 2048u == hpsize ? total : 0u;
    }

    bool is_hyper_threading_active() noexcept
    {
        std::ifstream file("/sys/devices/system/cpu/smt/active");

        std::string value;
        file >> value;

        return value != "0";
    }

    bool is_swap_active() noexcept
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

    bool is_transparent_hugepages_active() noexcept
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

    bool is_scaling_governor_use_performance_mode() noexcept
    {
        if (not std::filesystem::exists("/sys/devices/system/cpu/cpufreq"))
        {
            return true;
        }

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

namespace ihft::platform
{
    bool set_current_thread_name(const char*) noexcept
    {
        return true;
    }

    bool set_current_thread_cpu(unsigned) noexcept
    {
        return true;
    }

    bool reset_current_thread_cpu() noexcept
    {
        return true;
    }

    bool lock_memory_pages(bool, bool) noexcept
    {
        return true;
    }

    bool get_cpu_isolation_status(unsigned) noexcept
    {
        return false;
    }

    bool get_cpu_nohz_full_status(unsigned) noexcept
    {
        return false;
    }

    bool get_cpu_rcu_nocbs_status(unsigned) noexcept
    {
        return false;
    }

    unsigned total_1gb_hugepages() noexcept
    {
        return 0;
    }

    unsigned total_2mb_hugepages() noexcept
    {
        return 0;
    }

    bool is_hyper_threading_active() noexcept
    {
        return true;
    }

    bool is_swap_active() noexcept
    {
        return true;
    }

    bool is_transparent_hugepages_active() noexcept
    {
        return true;
    }

    bool is_scaling_governor_use_performance_mode() noexcept
    {
        return false;
    }
}

#endif
