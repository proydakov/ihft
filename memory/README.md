# ihft::memory

This module contains the code of domain-specific allocators.

[stream_fixed_pool_allocator](include/memory/stream_fixed_pool_allocator.h) is your best friend in working with fixed-size queues for non-POD objects. It contains one element more than the queue size, which allows you to fill the channel with data entirely and safely prepare the next batch of data.

[page_allocator](include/memory/page_allocator.h) and [huge_page_allocator](include/memory/huge_page_allocator.h) are simple unix page allocators. They can be useful for experiments with TLB.

[arena_allocator](include/memory/arena_allocator.h) is simple arena allocator. It allows you to pack arbitrary data of different types in a specific location.

```cpp
using alloc_t = arena_allocator::typed_arena_allocator<char>;
using astring = std::basic_string<char, std::char_traits<char>, alloc_t>;

struct network_data final
{
    network_data(std::string_view iname, std::string_view ilocation)
        : arena(extra_data)
        , name(iname, arena.typed_allocator<char>())
        , location(ilocation, arena.typed_allocator<char>())
    {
    }

    arena_allocator arena;

    astring name;
    astring location;

    char extra_data[192];
};

network_data data("Proydakov Evgeny Alexandrovich", "Moscow");
REQUIRE(data.name == "Proydakov Evgeny Alexandrovich");
REQUIRE(data.location == "Moscow");

REQUIRE(std::string_view(data.extra_data, 30) == "Proydakov Evgeny Alexandrovich");
```

## Benchmarks

[arena_allocator benchmark](benchmark/benchmark_arena_allocator.cpp)

[stream_fixed_pool_allocator benchmark](benchmark/benchmark_stream_fixed_pool_allocator.cpp)
