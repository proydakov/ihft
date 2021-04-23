#include <thread>
#include <cstdlib>
#include <iomanip>
#include <iostream>

#include <platform/platform.h>

using plf = ihft::platform;

int main(int, char*[])
{
    for(unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        std::cout << "cpu: " << std::setw(3) << i
            << " isolation: " << std::boolalpha << plf::get_cpu_isolation_status(i)
            << std::endl;
    }

    return EXIT_SUCCESS;
}
