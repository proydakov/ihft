/*

-------------------------------------------------------------------------------
call plain_function benchmark
-------------------------------------------------------------------------------
../types/benchmark/benchmark_function_ref.cpp:24
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
plain_function(1)                              100         31419     3.1419 ms
                                        1.32242 ns    1.30074 ns    1.36775 ns
                                       0.152832 ns  0.0763006 ns   0.275035 ns

-------------------------------------------------------------------------------
call function_ref for plain_function benchmark
-------------------------------------------------------------------------------
../types/benchmark/benchmark_function_ref.cpp:34
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
function_ref(1)                                100         20879     2.0879 ms
                                        1.82824 ns    1.82418 ns    1.84074 ns
                                      0.0326369 ns  0.0115059 ns  0.0703377 ns

-------------------------------------------------------------------------------
call std_function for plain_function benchmark
-------------------------------------------------------------------------------
../types/benchmark/benchmark_function_ref.cpp:46
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
std_function(1)                                100         17277     3.4554 ms
                                        2.37715 ns    2.27355 ns    2.53477 ns
                                        0.64232 ns   0.466241 ns    0.87302 ns

-------------------------------------------------------------------------------
construct + call function_ref for compact_lambda benchmark
-------------------------------------------------------------------------------
../types/benchmark/benchmark_function_ref.cpp:77
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
construct function_ref()                       100         17091     3.4182 ms
                                        2.40609 ns    2.31412 ns     2.5528 ns
                                       0.583353 ns   0.408288 ns    0.81632 ns

-------------------------------------------------------------------------------
construct + call std_function for compact_lambda benchmark
-------------------------------------------------------------------------------
../types/benchmark/benchmark_function_ref.cpp:88
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
construct std_function()                       100         50715          0 ns
                                       0.761417 ns   0.759938 ns   0.767547 ns
                                      0.0126874 ns 0.000728316 ns 0.0296316 ns

-------------------------------------------------------------------------------
construct + call function_ref for huge_lambda benchmark
-------------------------------------------------------------------------------
../types/benchmark/benchmark_function_ref.cpp:123
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
construct function_ref()                       100         18595      3.719 ms
                                        1.95861 ns    1.87984 ns    2.11864 ns
                                       0.546082 ns   0.326653 ns   0.946482 ns

-------------------------------------------------------------------------------
construct + call std_function for huge_lambda benchmark
-------------------------------------------------------------------------------
../types/benchmark/benchmark_function_ref.cpp:134
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
construct std_function()                       100          1091     3.7094 ms
                                        34.6758 ns    34.5399 ns    35.3343 ns
                                        1.31991 ns  0.0514149 ns    3.14821 ns

===============================================================================
test cases: 7 | 7 passed
assertions: - none -

*/

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <compiler/compiler.h>
#include <types/function_ref.h>

#include <functional>

namespace
{
    // call types
    using function_ref = ihft::types::function_ref<uint64_t(uint64_t&, uint64_t)>;
    using function_std = std::function<uint64_t(uint64_t&, uint64_t)>;

    // simple plain_function

    IHFT_NOINLINE uint64_t plain_function(uint64_t& origin, uint64_t val)
    {
        origin += val;
        return origin;
    }
}

TEST_CASE("call plain_function benchmark")
{
    uint64_t counter = 0;

    BENCHMARK("plain_function(1)")
    {
        return plain_function(counter, 1);
    };
}

TEST_CASE("call function_ref for plain_function benchmark")
{
    function_ref plaing_function_ref{plain_function};

    uint64_t counter = 0;

    BENCHMARK("function_ref(1)")
    {
        return plaing_function_ref(counter, 1);
    };
}

TEST_CASE("call std_function for plain_function benchmark")
{
    function_std plain_std_function{plain_function};

    uint64_t counter = 0;

    BENCHMARK("std_function(1)")
    {
        return plain_std_function(counter, 1);
    };
}

namespace
{
    // complex lambda function with compact storage content

    constexpr uint64_t c{};

    static auto compact_lambda = [v1 = c, v2 = c, v3 = c]
        (uint64_t& o, uint64_t d) mutable -> uint64_t
    {
        v1 += d;
        v2 += d;
        v3 += d;

        o += v1 + v2 + v3;

        return 0;
    };
}

TEST_CASE("construct + call function_ref for compact_lambda benchmark")
{
    uint64_t counter = 0;

    BENCHMARK("construct function_ref()")
    {
        function_ref plaing_function_ref{compact_lambda};
        return plaing_function_ref(counter, 1);
    };
}

TEST_CASE("construct + call std_function for compact_lambda benchmark")
{
    uint64_t counter = 0;

    BENCHMARK("construct std_function()")
    {
        function_std plain_std_function{compact_lambda};
        return plain_std_function(counter, 1);
    };
}

namespace
{
    // complex lambda function with huge storage content

    constexpr uint64_t h{};

    static auto huge_lambda = [v1 = h, v2 = h, v3 = h, v4 = h, v5 = h, v6 = h, v7 = h, v8 = h]
        (uint64_t& o, uint64_t d) mutable -> uint64_t
    {
        v1 += d;
        v2 += d;
        v3 += d;
        v4 += d;
        v5 += d;
        v6 += d;
        v7 += d;
        v8 += d;

        o += v1 + v2 + v3 + v4 + v5 + v6 + v7 + v8;

        return 0;
    };
}

TEST_CASE("construct + call function_ref for huge_lambda benchmark")
{
    uint64_t counter = 0;

    BENCHMARK("construct function_ref()")
    {
        function_ref plaing_function_ref{huge_lambda};
        return plaing_function_ref(counter, 1);
    };
}

TEST_CASE("construct + call std_function for huge_lambda benchmark")
{
    uint64_t counter = 0;

    BENCHMARK("construct std_function()")
    {
        function_std plain_std_function{huge_lambda};
        return plain_std_function(counter, 1);
    };
}
