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
    REQUIRE(ihft::platform::set_current_thread_name("test"));
}

TEST_CASE("set_current_thread_cpu")
{
    REQUIRE(ihft::platform::set_current_thread_cpu(0));
}

TEST_CASE("reset_current_thread_cpu")
{
    REQUIRE(ihft::platform::reset_current_thread_cpu());
}

TEST_CASE("lock_memory_pages")
{
    // Github CI platforms has problem with mlockall.
    // I decided to skip this test in CI env.
    (ihft::platform::lock_memory_pages(true, true));
}
