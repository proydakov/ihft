#include <thread>
#include <cstdlib>
#include <iostream>

#include <platform/platform.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " <cpu>" << std::endl;
        return EXIT_FAILURE;
    }

    std::thread thread([argv](){
        ihft::platform::set_current_thread_cpu(std::stoul(argv[1]));
        std::cout << "Enter any symbol for exit... ";
        char c;
        std::cin >> c;
    });

    thread.join();

    return EXIT_SUCCESS;
}
