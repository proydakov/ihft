#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <timer/timer.h>

/*

% sysctl -n machdep.cpu.brand_string

Intel(R) Core(TM) i5-1038NG7 CPU @ 2.00GHz

-------------------------------------------------------------------------------
cpu_counter benchmark
-------------------------------------------------------------------------------
../timer/benchmark/benchmark_timer.cpp:8
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
cpu_counter()                                  100          5231     3.6617 ms
                                        7.35063 ns    7.12136 ns    8.05525 ns
                                        1.72034 ns 0.00152105 ns    3.80921 ns

-------------------------------------------------------------------------------
cpu_pause benchmark
-------------------------------------------------------------------------------
../timer/benchmark/benchmark_timer.cpp:16
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
cpu_pause()                                    100           652     3.7816 ms
                                        45.1592 ns    43.0884 ns    47.5848 ns
                                        11.4076 ns    10.1455 ns    14.7173 ns

*/

using namespace ihft::timer;

TEST_CASE("cpu_counter benchmark")
{
    BENCHMARK("cpu_counter()")
    {
        return cpu_counter();
    };
}

TEST_CASE("cpu_pause benchmark")
{
    BENCHMARK("cpu_pause()")
    {
        return cpu_pause();
    };
}
