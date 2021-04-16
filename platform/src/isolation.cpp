#include <platform/private/isolation.h>

#include <cstring>
#include <fstream>
#include <charconv>
#include <string_view>

namespace ihft::impl
{

static_assert(sizeof(isolation) == 64);

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
                    // only 1 cpu in section
                    if (p == cpu.end())
                    {
                        m_isolated.set(result);
                    }
                    // range cpu in section
                    else
                    {
                        unsigned long to_result;
                        if(auto [to_p, to_ec] = std::from_chars(p + 1, cpu.end(), to_result); to_ec == std::errc())
                        {
                            for(auto i = result; i <= to_result; i++)
                            {
                                m_isolated.set(i);
                            }
                        }

                    }
                }
            }
            while(next != std::string_view::npos);
        }
    }
}

}
