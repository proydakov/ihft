#include <engine/private/logical_cpu.h>
#include <platform/platform.h>

#include <string>
#include <chrono>
#include <cstdlib>
#include <iostream>

using logical_cpu = ihft::engine::impl::logical_cpu_impl<ihft::platform::trait>;

std::chrono::nanoseconds calc_cpu_cycle()
{
    auto res = std::chrono::nanoseconds::max();

    auto t1 = std::chrono::steady_clock::now();
    for(int i = 0; i < 1'000'000'000; i++)
    {
        auto const t2 = std::chrono::steady_clock::now();
        auto const delta = t2 - t1;
        if (delta < res)
        {
            res = delta;
        }
    }

    return res;
}

int main(int const argc, char const * const argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage. engine_logical_cpu_demo <cpu_id> <cpu_name>" << std::endl;
        return EXIT_FAILURE;
    }

    auto const cpu_id = static_cast<unsigned>(std::stoul(argv[1]));
    auto const cpu_name = argv[2];

    std::chrono::nanoseconds delta1;
    std::chrono::nanoseconds delta2;

    {
        std::cout << "step1" << std::endl;

        logical_cpu cpu(cpu_id, cpu_name);
        if (not cpu.bind())
        {
            std::cout << "Can't bind <cpu_id> <cpu_name>" << cpu_id << " " << cpu_name << std::endl;
            return EXIT_FAILURE;
        }

        delta1 = calc_cpu_cycle();
    }

    {
        std::cout << "step2" << std::endl;

        delta2 = calc_cpu_cycle();
    }

    auto const min_delta = std::min(delta1, delta2);

    std::cout << "min_delta: " << min_delta.count() << std::endl;

    return EXIT_SUCCESS;
}
