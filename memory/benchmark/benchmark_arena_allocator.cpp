#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <memory/arena_allocator.h>

#include <vector>
#include <type_traits>

/*

-------------------------------------------------------------------------------
arena_allocator benchmark
-------------------------------------------------------------------------------
../memory/benchmark/benchmark_arena_allocator.cpp:38
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
allocate(1)                                    100         41473          0 ns
                                       0.928058 ns   0.901819 ns    1.00861 ns
                                       0.198474 ns  0.0112234 ns    0.42777 ns
-------------------------------------------------------------------------------

 */

using namespace ihft::memory;

struct A
{
    std::uint64_t type{};
    std::uint64_t length{};
};

static_assert(std::is_same_v<arena_allocator::typed_arena_allocator<A>::value_type, A>, "value_type should works correctly");

std::vector<char> g_memory_slab(1024ul * 1024ul * 1024ul);

TEST_CASE("arena_allocator benchmark")
{
    arena_allocator arena(g_memory_slab.data(), g_memory_slab.size());
    auto allocator = arena.typed_allocator<A>();

    BENCHMARK("allocate(1)")
    {
        auto ptr = allocator.allocate(1);
        if (ptr == nullptr) [[unlikely]]
        {
            arena.reset();
            ptr = allocator.allocate(1);
        }
        return ptr;
    };
}
