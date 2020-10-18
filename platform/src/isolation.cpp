#include <platform/private/isolation.h>

#include <string.h>
#include <fstream>
#include <charconv>
#include <algorithm>
#include <string_view>

namespace ihft::impl
{

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
            size_t from = 0;
            size_t next = 0;
            do
            {
                next = cpus.find(',', from);
                auto const cpu = cpus.substr(from, next - from);
                from = next + 1;

                unsigned long result;
                if(auto [p, ec] = std::from_chars(cpu.begin(), cpu.end(), result); ec == std::errc())
                {
                    m_isolated.emplace_back(result);
                }
            }
            while(next != std::string_view::npos);
        }
    }

    std::sort(m_isolated.begin(), m_isolated.end());
}

bool isolation::is_isolated(unsigned long cpu) const
{
    return std::binary_search(m_isolated.begin(), m_isolated.end(), cpu);
}

}
