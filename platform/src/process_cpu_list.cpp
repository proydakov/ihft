#include <platform/process_cpu_list.h>

#include <charconv>

namespace ihft::platform
{

void process_cpu_list(std::string_view cpus, std::function<void(unsigned)> const& functor)
{
    size_t from{};
    size_t next{};

    do
    {
        next = cpus.find(',', from);
        auto const cpu = cpus.substr(from, (next != std::string_view::npos) ? (next - from) : std::string_view::npos);
        from = next + 1;

        unsigned result{};
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
                if ('-' != *p)
                {
                    return;
                }

                unsigned to_result{};
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
