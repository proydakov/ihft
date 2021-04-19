#include "catch2/catch.hpp"

#include <memory/stream_fixed_pool_allocator.h>

#include <set>

using namespace ihft;

struct A
{
    std::uint64_t type{};
    std::uint64_t length{};
};

TEST_CASE("test_stream_fixed_pool_allocator")
{
    constexpr std::size_t size = 16;

    stream_fixed_pool_allocator<A> allocator(size);

    std::set<A*> set;

    // size + 1 extra slab
    for(std::size_t i = 0; i < size + 1; i++)
    {
        set.insert(allocator.allocate(1));

        REQUIRE( set.size() == i + 1 );
    }

    set.insert(allocator.allocate(1));

    REQUIRE( set.size() == size + 1 );
}

TEST_CASE("stream_fixed_pool_allocator array allocation")
{
    stream_fixed_pool_allocator<A> allocator(16);

    REQUIRE( allocator.allocate(2) == nullptr );
}
