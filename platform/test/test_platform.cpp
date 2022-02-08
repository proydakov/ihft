#include <catch2/catch.hpp>

#include <platform/platform.h>

#include <thread>

TEST_CASE("get_cpu_isolation_status")
{
    for(unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        ihft::platform::get_cpu_isolation_status(i);
    }
}

TEST_CASE("set_current_thread_name")
{
    ihft::platform::set_current_thread_name("test");
}

TEST_CASE("set_current_thread_cpu")
{
    ihft::platform::set_current_thread_cpu(0);
}
