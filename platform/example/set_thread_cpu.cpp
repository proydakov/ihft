#include <platform/platform.h>

#include <thread>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " <cpu>" << std::endl;
        return EXIT_FAILURE;
    }

    std::thread thread([argv](){
        auto const cpu = static_cast<unsigned>(std::stoul(argv[1]));
        ihft::platform::trait::set_current_thread_cpu(cpu);
        std::cout << "Enter any symbol for exit... ";
        char c;
        std::cin >> c;
    });

    thread.join();

    return EXIT_SUCCESS;
}
