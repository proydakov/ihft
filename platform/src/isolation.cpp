#include <platform/private/isolation.h>
#include <platform/process_cpu_list.h>
#include <constant/constant.h>

#include <cstring>
#include <fstream>
#include <charconv>
#include <string_view>

namespace ihft::impl
{

static_assert(sizeof(isolation) == constant::CPU_CACHE_LINE_SIZE);

isolation::isolation(const char* file)
{
    std::ifstream input(file);

    std::string text;
    while(input >> text)
    {
        const char * const pattern = "isolcpus=";
        auto const index = text.find(pattern);
        if (index != std::string::npos)
        {
            auto const cpus = std::string_view(text).substr(index + strlen(pattern));
            auto lambda = [&](unsigned cpu) mutable
            {
                m_isolated.set(cpu);
            };
            process_cpu_list(cpus, lambda);
        }
    }
}

}
