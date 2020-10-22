#include <platform/platform.h>

#include <thread>

int test_1()
{
    for(unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        ihft::platform::get_cpu_isolation_status(i);
    }

    return 0;
}

int test_2()
{
    ihft::platform::set_current_thread_name("test");

    return 0;
}

int test_3()
{
    ihft::platform::set_current_thread_cpu(0);

    return 0;
}

int main(int, char const*[])
{
    return test_1() + test_2() + test_3();
}
