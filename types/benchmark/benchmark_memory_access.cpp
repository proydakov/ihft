#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <compiler/compiler.h>

#include <functional>

namespace
{
static int64_t g_counter{};
}

IHFT_NOINLINE int64_t accumulate_global()
{
    g_counter++;
    return g_counter;
}

TEST_CASE("global_variable benchmark")
{
    BENCHMARK("accumulate_global()")
    {
        return accumulate_global();
    };
}

namespace
{
static thread_local int64_t g_tl_counter{};
}

IHFT_NOINLINE int64_t accumulate_thread_local()
{
    g_tl_counter++;
    return g_tl_counter;
}

TEST_CASE("thread_local_variable benchmark")
{
    BENCHMARK("accumulate_thread_local()")
    {
        return accumulate_thread_local();
    };
}

IHFT_NOINLINE int64_t accumulate_by_reference(int64_t& ref)
{
    ref++;
    return ref;
}

TEST_CASE("stack_local_variable benchmark")
{
    int64_t var{};
    BENCHMARK("accumulate_be_reference()")
    {
        return accumulate_by_reference(var);
    };
}

