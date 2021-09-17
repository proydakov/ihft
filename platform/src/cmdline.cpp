#include <platform/private/cmdline.h>
#include <platform/process_cpu_list.h>
#include <constant/constant.h>

#include <cstring>
#include <fstream>
#include <charconv>
#include <string_view>

namespace ihft::impl
{

static_assert(sizeof(cmdline) == 3 * constant::CPU_CACHE_LINE_SIZE);

cmdline::cmdline(const char* file)
{
    std::ifstream input(file);

    auto process = [](auto const& text, auto const pattern, auto& list)
    {
        auto const index = text.find(pattern);
        if (index != std::string::npos)
        {
            auto const cpus = std::string_view(text).substr(index + strlen(pattern));
            auto lambda = [&](unsigned cpu) mutable
            {
                list.set(cpu);
            };
            ihft::platform::process_cpu_list(cpus, lambda);
        }
    };

    const char * const isolcpus_pattern = "isolcpus=";
    const char * const nohz_full_pattern = "nohz_full=";
    const char * const rcu_nocbs_pattern = "rcu_nocbs=";

    std::string text;
    while(input >> text)
    {
        process(text, isolcpus_pattern, m_isolated);
        process(text, nohz_full_pattern, m_nohz_fulled);
        process(text, rcu_nocbs_pattern, m_rcu_nocbsed);
    }
}

}
