/*

-------------------------------------------------------------------------------
char_cmp benchmark
-------------------------------------------------------------------------------
../types/benchmark/benchmark_char
_vs_strcmp.cpp:13
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
char_cmp('A', 'a')                             100          7990      5.593 ms 
                                        7.01381 ns    6.79556 ns    7.41639 ns 
                                        1.46849 ns   0.949145 ns    2.33642 ns 
                                                                               

-------------------------------------------------------------------------------
str_cmp benchmark
-------------------------------------------------------------------------------
../types/benchmark/benchmark_char_vs_strcmp.cpp:29
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
str_cmp("A", "a")                              100          6514     5.8626 ms 
                                        10.2284 ns    9.76705 ns    11.0137 ns 
                                        3.00717 ns      1.957 ns    4.27222 ns 
                                                                               

===============================================================================
test cases: 2 | 2 passed
assertions: - none -

*/

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <compiler/compiler.h>
#include <cstring>

IHFT_NOINLINE bool char_cmp(char c1, char c2)
{
    return c1 == c2;
}

TEST_CASE("char_cmp benchmark")
{
    const char c1 = 'A';
    const char c2 = 'a';

    BENCHMARK("char_cmp('A', 'a')")
    {
        return char_cmp(c1, c2);
    };
}

IHFT_NOINLINE bool str_cmp(const char * const s1, const char * const s2)
{
    return 0 == strcmp(s1, s2);
}

TEST_CASE("str_cmp benchmark")
{
    const char* const s1 = "A";
    const char* const s2 = "a";

    BENCHMARK("str_cmp(\"A\", \"a\")")
    {
        return str_cmp(s1, s2);
    };
}
