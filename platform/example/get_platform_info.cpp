#include <platform/platform.h>

#include <thread>
#include <cstdlib>
#include <iomanip>
#include <iostream>

using plf = ihft::platform;

int main(int, char*[])
{
    std::cout << "IHFT platform information\n" << std::endl;

    std::cout << "Is CPU frequency scaling governor use performance: " << std::boolalpha << plf::is_scaling_governor_use_performance_mode() << std::endl;
    std::cout << "Total 1GB hugepages: " << plf::total_1gb_hugepages() << std::endl;
    std::cout << "Total 2MB hugepages: " << plf::total_2mb_hugepages() << std::endl;

    std::cout << "\n";

    std::cout << "SMT is active: " << std::boolalpha << plf::is_smt_active() << std::endl;
    std::cout << "Swap is active: " << std::boolalpha << plf::is_swap_active() << std::endl;
    std::cout << "Transparent_hugepage is active: " << std::boolalpha << plf::is_transparent_hugepages_active() << std::endl;

    std::cout << "\n";

    for(unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        std::cout << "cpu: " << std::setw(3) << i
            << " isolation: " << std::boolalpha << plf::get_cpu_isolation_status(i) << std::endl;
    }

    return EXIT_SUCCESS;
}
