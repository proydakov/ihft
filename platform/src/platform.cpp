#include <platform/platform.h>

#include <thread>

namespace
{
    static const unsigned g_cpus{std::thread::hardware_concurrency()};
}

#ifdef __linux__

#include <platform/private/cmdline.h>

#include <cstring>
#include <fstream>
#include <charconv>
#include <filesystem>
#include <string_view>

#include <sched.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/prctl.h>

namespace
{
    static const ihft::impl::cmdline g_cmdline("/proc/cmdline");

    static const cpu_set_t g_default_cpuset = []()
    {
        cpu_set_t set;
        sched_getaffinity(0, sizeof(cpu_set_t), &set);
        return set;
    }();

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

#endif

namespace ihft::platform
{

    unsigned trait::get_total_cpus() noexcept
    {
        return g_cpus;
    }

#ifdef __linux__

    long trait::get_thread_id() noexcept
    {
        return gettid();
    }

    bool trait::set_current_thread_name(const char* name) noexcept
    {
        return 0 == prctl(PR_SET_NAME, name, 0, 0, 0);
    }

    bool trait::set_current_thread_cpu(unsigned cpu) noexcept
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(static_cast<unsigned long>(cpu), &cpuset);

        return 0 == sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
    }

    bool trait::reset_current_thread_cpu() noexcept
    {
        return 0 == sched_setaffinity(0, sizeof(cpu_set_t), &g_default_cpuset);
    }

    bool trait::lock_memory_pages(bool current, bool future) noexcept
    {
        int flags = 0;

        if (current)
            flags |= MCL_CURRENT;
        if (future)
            flags |= MCL_FUTURE;

        return 0 == mlockall(flags);
    }

    bool trait::get_cpu_isolation_status(unsigned cpu) noexcept
    {
        return g_cmdline.is_isolated(cpu);
    }

    bool trait::get_cpu_nohz_full_status(unsigned cpu) noexcept
    {
        return g_cmdline.is_nohz_fulled(cpu);
    }

    bool trait::get_cpu_rcu_nocbs_status(unsigned cpu) noexcept
    {
        return g_cmdline.is_rcu_nocbsed(cpu);
    }

    unsigned trait::total_1gb_hugepages() noexcept
    {
        auto const [total, hpsize] = get_hp_info_impl();

        return 1048576u == hpsize ? total : 0u;
    }

    unsigned trait::total_2mb_hugepages() noexcept
    {
        auto const [total, hpsize] = get_hp_info_impl();

        return 2048u == hpsize ? total : 0u;
    }

    bool trait::is_hyper_threading_active() noexcept
    {
        std::ifstream file("/sys/devices/system/cpu/smt/active");

        std::string value;
        file >> value;

        return value != "0";
    }

    bool trait::is_swap_active() noexcept
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

    bool trait::is_transparent_hugepages_active() noexcept
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

    bool trait::is_scaling_governor_use_performance_mode() noexcept
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

#else

    long trait::get_thread_id() noexcept
    {
        uint64_t pth_threadid = 0;
        pthread_threadid_np(nullptr, &pth_threadid);
        return static_cast<long>(pth_threadid);
    }

    bool trait::set_current_thread_name(const char*) noexcept
    {
        return true;
    }

    bool trait::set_current_thread_cpu(unsigned) noexcept
    {
        return true;
    }

    bool trait::reset_current_thread_cpu() noexcept
    {
        return true;
    }

    bool trait::lock_memory_pages(bool, bool) noexcept
    {
        return true;
    }

    bool trait::get_cpu_isolation_status(unsigned) noexcept
    {
        return false;
    }

    bool trait::get_cpu_nohz_full_status(unsigned) noexcept
    {
        return false;
    }

    bool trait::get_cpu_rcu_nocbs_status(unsigned) noexcept
    {
        return false;
    }

    unsigned trait::total_1gb_hugepages() noexcept
    {
        return 0;
    }

    unsigned trait::total_2mb_hugepages() noexcept
    {
        return 0;
    }

    bool trait::is_hyper_threading_active() noexcept
    {
        return true;
    }

    bool trait::is_swap_active() noexcept
    {
        return true;
    }

    bool trait::is_transparent_hugepages_active() noexcept
    {
        return true;
    }

    bool trait::is_scaling_governor_use_performance_mode() noexcept
    {
        return false;
    }

#endif

}
