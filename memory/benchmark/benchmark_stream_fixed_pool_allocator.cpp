#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <memory/stream_fixed_pool_allocator.h>

#include <type_traits>

/*

-------------------------------------------------------------------------------
../memory/benchmark/benchmark_stream_fixed_pool_allocator.cpp:18
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
allocate(1)                                    100         36774     3.6774 ms
                                        1.13102 ns    1.12934 ns    1.13343 ns
                                      0.0101578 ns 0.00741728 ns  0.0169453 ns
-------------------------------------------------------------------------------

 */

using namespace ihft;

struct A
{
    std::uint64_t type{};
    std::uint64_t length{};
};

static_assert(std::is_same_v<stream_fixed_pool_allocator<A>::value_type, A>, "value_type should works correctly");

TEST_CASE("stream_fixed_pool_allocator benchmark")
{
    stream_fixed_pool_allocator<A> allocator(1024);

    BENCHMARK("allocate(1)")
    {
        return allocator.allocate(1);
    };
}
