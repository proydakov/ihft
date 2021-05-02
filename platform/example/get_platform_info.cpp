#include <platform/platform.h>

#include <thread>
#include <cstdlib>
#include <iomanip>
#include <iostream>

using plf = ihft::platform;

int main(int, char*[])
{
    std::cout << "IHFT platform information\n" << std::endl;

    std::cout << "SMT is active: " << std::boolalpha << plf::is_smt_active() << std::endl;
    std::cout << "Swap is active: " << std::boolalpha << plf::is_swap_active() << std::endl;
    std::cout << "Transparent_hugepage is active: " << std::boolalpha << plf::is_transparent_huge_pages_active() << std::endl;

    std::cout << "Is CPU frequency scaling governor use performance: " << std::boolalpha << plf::is_scaling_governor_use_performance_mode() << std::endl;

    std::cout << std::endl;

    for(unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        std::cout << "cpu: " << std::setw(3) << i
            << " isolation: " << std::boolalpha << plf::get_cpu_isolation_status(i)
            << std::endl;
    }

    return EXIT_SUCCESS;
}
