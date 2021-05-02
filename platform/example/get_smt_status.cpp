#include <platform/platform.h>

#include <iomanip>
#include <iostream>

using plf = ihft::platform;

int main(int, char*[])
{
    std::cout << "SMT is active: " << std::boolalpha << plf::is_smt_active() << std::endl;

    return EXIT_SUCCESS;
}
