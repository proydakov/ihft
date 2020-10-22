#include <thread>
#include <cstdlib>
#include <iostream>

#include <platform/platform.h>

int main(int, char*[])
{
    for(unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        std::cout << "cpu: " << i << " isolation: " << ihft::platform::get_cpu_isolation_status(i) << std::endl;
    }

    return EXIT_SUCCESS;
}
