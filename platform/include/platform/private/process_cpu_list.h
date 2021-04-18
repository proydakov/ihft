#pragma once

#include <charconv>
#include <string_view>

namespace ihft::impl
{

template<typename Functor>
void process_cpu_list(std::string_view cpus, Functor& functor)
{
    size_t from = 0;
    size_t next = 0;
    do
    {
        next = cpus.find(',', from);
        auto const cpu = cpus.substr(from, next - from);
        from = next + 1;

        unsigned result;
        if(auto [p, ec] = std::from_chars(cpu.begin(), cpu.end(), result); ec == std::errc())
        {
            // only 1 cpu in section
            if (p == cpu.end())
            {
                functor(result);
            }
            // range cpu in section
            else
            {
                unsigned to_result;
                if(auto [to_p, to_ec] = std::from_chars(p + 1, cpu.end(), to_result); to_ec == std::errc())
                {
                    for(auto i = result; i <= to_result; i++)
                    {
                        functor(i);
                    }
                }
            }
        }
    }
    while(next != std::string_view::npos);
}

}
