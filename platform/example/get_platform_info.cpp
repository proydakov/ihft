#include <platform/platform.h>

#include <thread>
#include <cstdlib>
#include <iomanip>
#include <iostream>

using plf = ihft::platform;

int main(int, char*[])
{
    std::cout << "IHFT platform information\n" << std::endl;

    std::cout << "Is CPU scaling governor use performance: " << std::boolalpha << plf::is_scaling_governor_use_performance_mode() << std::endl;
    std::cout << "Total 1GB hugepages: " << plf::total_1gb_hugepages() << std::endl;
    std::cout << "Total 2MB hugepages: " << plf::total_2mb_hugepages() << std::endl;

    std::cout << "\n";

    std::cout << "Is hyper threading active: " << std::boolalpha << plf::is_hyper_threading_active() << std::endl;
    std::cout << "Is swap active: " << std::boolalpha << plf::is_swap_active() << std::endl;
    std::cout << "Is transparent_hugepage active: " << std::boolalpha << plf::is_transparent_hugepages_active() << std::endl;

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
