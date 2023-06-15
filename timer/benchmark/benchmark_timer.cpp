#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <timer/timer.h>

using namespace ihft::timer;

TEST_CASE("cpu_counter benchmark")
{
    BENCHMARK("cpu_counter()")
    {
        return cpu_counter();
    };
}
