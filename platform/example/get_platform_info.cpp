//
// A tool to get linux system configuration details for low latency applications.
//

#include <platform/platform.h>

#include <thread>
#include <cstdlib>
#include <iomanip>
#include <iostream>

namespace plf = ihft::platform;

int main()
{
    std::cout << "IHFT platform information\n" << std::endl;

    std::cout << "Hugepage allocator info (how many pages are available):" << std::endl;
    std::cout << "Total 1GB hugepages: " << plf::total_1gb_hugepages() << std::endl;
    std::cout << "Total 2MB hugepages: " << plf::total_2mb_hugepages() << std::endl;

    std::cout << "\n";

    std::cout << "Positive factors (should be true):" << std::endl;
    std::cout << "Is CPU scaling governor use performance: " << std::boolalpha << plf::is_scaling_governor_use_performance_mode() << std::endl;

    std::cout << "\n";

    std::cout << "Negative factors (should be false):" << std::endl;
    std::cout << "Is hyper threading active: " << std::boolalpha << plf::is_hyper_threading_active() << std::endl;
    std::cout << "Is transparent_hugepage active: " << std::boolalpha << plf::is_transparent_hugepages_active() << std::endl;
    std::cout << "Is swap active: " << std::boolalpha << plf::is_swap_active() << std::endl;

    std::cout << "\n";

    std::cout << "| cpu | isolation | nohz_full | rcu_nocbs |" << std::endl;
    for(unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        std::cout << std::noboolalpha
            << std::setw(5) << i
            << std::setw(12) << (plf::get_cpu_isolation_status(i) ? '*' : ' ')
            << std::setw(12) << (plf::get_cpu_nohz_full_status(i) ? '*' : ' ')
            << std::setw(12) << (plf::get_cpu_rcu_nocbs_status(i) ? '*' : ' ')
            << std::endl;
    }

    return EXIT_SUCCESS;
}
