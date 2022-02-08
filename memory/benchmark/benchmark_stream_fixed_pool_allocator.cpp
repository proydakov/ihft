#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <memory/stream_fixed_pool_allocator.h>

#include <type_traits>

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
