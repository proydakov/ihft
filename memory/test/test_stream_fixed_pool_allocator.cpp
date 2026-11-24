#include <catch2/catch_test_macros.hpp>

#include <memory/stream_fixed_pool_allocator.h>

#include <set>
#include <type_traits>
#include <utility>

using namespace ihft::memory;

struct A
{
    std::uint64_t type{};
    std::uint64_t length{};
};

static_assert(std::is_same_v<stream_fixed_pool_allocator<A>::value_type, A>, "value_type should works correctly");

TEST_CASE("test_stream_fixed_pool_allocator STL-api")
{
    constexpr std::size_t size = 16;

    stream_fixed_pool_allocator<A> allocator(size);

    REQUIRE(allocator.capacity() == 17);

    std::set<A*> set;

    // size + 1 extra slab
    for(std::size_t i = 0; i < size + 1; i++)
    {
        REQUIRE(allocator.position() == i);

        set.insert(allocator.allocate(1));

        REQUIRE( set.size() == i + 1 );
    }

    set.insert(allocator.allocate(1));

    REQUIRE( set.size() == size + 1 );
}

TEST_CASE("test_stream_fixed_pool_allocator IHFT-api")
{
    constexpr std::size_t size = 16;

    stream_fixed_pool_allocator<A> allocator(size);

    std::set<A*> set;

    for(std::size_t i = 0; i < 8; i++)
    {
        set.insert(allocator.active_slab());

        REQUIRE( set.size() == 1 );
    }

    allocator.seek_to_next_slab();
    set.insert(allocator.active_slab());

    REQUIRE( set.size() == 2 );
}

TEST_CASE("stream_fixed_pool_allocator element allocation")
{
    stream_fixed_pool_allocator<A> allocator(16);

    REQUIRE( allocator.allocate(1) != nullptr );
}

TEST_CASE("stream_fixed_pool_allocator array allocation")
{
    stream_fixed_pool_allocator<A> allocator(16);

    REQUIRE( allocator.allocate(2) == nullptr );
}

TEST_CASE("stream_fixed_pool_allocator move construction")
{
    stream_fixed_pool_allocator<A> source(16);

    source.seek_to_next_slab();
    source.seek_to_next_slab();

    auto* const slab = source.active_slab();
    auto const capacity = source.capacity();

    REQUIRE( source.position() == 2 );

    stream_fixed_pool_allocator<A> target(std::move(source));

    REQUIRE( target.capacity() == capacity );
    REQUIRE( target.position() == 2 );
    REQUIRE( target.active_slab() == slab );

    // The moved-from object no longer owns the region.
    REQUIRE( source.capacity() == 0 );
}

TEST_CASE("stream_fixed_pool_allocator move assignment")
{
    stream_fixed_pool_allocator<A> source(8);
    stream_fixed_pool_allocator<A> target(32);

    source.seek_to_next_slab();

    auto* const slab = source.active_slab();
    auto const capacity = source.capacity();

    target = std::move(source);

    REQUIRE( target.capacity() == capacity );
    REQUIRE( target.position() == 1 );
    REQUIRE( target.active_slab() == slab );

    // The moved-from object no longer owns the region.
    REQUIRE( source.capacity() == 0 );
}
