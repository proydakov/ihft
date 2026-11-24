#include <catch2/catch_test_macros.hpp>

#include <memory/arena_allocator.h>

#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <algorithm>
#include <string_view>

using namespace ihft::memory;

TEST_CASE("chars placing STL-api")
{
    char buffer[16] = { };
    std::fill(buffer, buffer + sizeof(buffer), ' ');

    arena_allocator arena(buffer, sizeof(buffer));
    auto typed1 = arena.typed_allocator<char>();
    auto p1 = typed1.allocate(6);
    p1[0] = 'H';
    p1[1] = 'e';
    p1[2] = 'l';
    p1[3] = 'l';
    p1[4] = 'o';
    p1[5] = ' ';

    auto typed2 = arena.typed_allocator<char>();
    auto p2 = typed2.allocate(6);
    p2[0] = 'w';
    p2[1] = 'o';
    p2[2] = 'r';
    p2[3] = 'l';
    p2[4] = 'd';
    p2[5] = '!';

    auto typed3 = arena.typed_allocator<char>();
    auto p3 = typed3.allocate(4);
    p3[0] = '_';
    p3[1] = '_';
    p3[2] = '_';
    p3[3] = '_';

    std::string_view const view(buffer, sizeof(buffer));
    std::string_view const etalon("Hello world!____");

    REQUIRE(view == etalon);

    auto typed5 = arena.typed_allocator<char>();
    auto p5 = typed5.allocate(1);
    REQUIRE(static_cast<void*>(p5) == nullptr);

    typed1.deallocate(p1, 6);
    typed2.deallocate(p2, 6);
    typed3.deallocate(p3, 4);
}

TEST_CASE("reset arena")
{
    char buffer[16] = { };
    std::fill(buffer, buffer + sizeof(buffer), ' ');

    arena_allocator arena(buffer, sizeof(buffer));

    auto typed1 = arena.typed_allocator<char>();
    auto p1 = typed1.allocate(sizeof(buffer));
    REQUIRE(static_cast<void*>(p1) == static_cast<void*>(buffer));

    auto p2 = typed1.allocate(1);
    REQUIRE(static_cast<void*>(p2) == nullptr);

    arena.reset();

    auto typed2 = arena.typed_allocator<char>();
    auto p3 = typed2.allocate(sizeof(buffer));
    REQUIRE(static_cast<void*>(p3) == static_cast<void*>(buffer));
}

TEST_CASE("uint64_t placing")
{
    alignas(2) unsigned char buffer[24] = { };
    std::fill(buffer, buffer + sizeof(buffer), 0);

    REQUIRE( (uint64_t)(buffer + 1) % 2 == 1);

    arena_allocator arena(buffer + 1, sizeof(buffer) - 1);
    auto typed1 = arena.typed_allocator<uint64_t>();

    auto p1 = typed1.allocate(1);

    REQUIRE(static_cast<void*>(buffer + sizeof(uint64_t)) == static_cast<void*>(p1));

    auto ptr = std::construct_at(p1, std::numeric_limits<uint64_t>::max());

    REQUIRE(*ptr == std::numeric_limits<uint64_t>::max());

    for(size_t i = 0; i < sizeof(buffer); i++)
    {
        if (i > 7 && i < 16)
        {
            REQUIRE(static_cast<unsigned>(buffer[i]) == 0xFF);
        }
        else
        {
            REQUIRE(static_cast<unsigned>(buffer[i]) == 0x00);
        }
    }

    std::destroy_at(p1);
}

TEST_CASE("struct with tail extra data")
{
    using alloc_t = arena_allocator::typed_arena_allocator<char>;
    using astring = std::basic_string<char, std::char_traits<char>, alloc_t>;

    struct network_data final
    {
        network_data(std::string_view iname, std::string_view ilocation)
            : arena(get_extra(), sizeof(extra_data))
            , name(iname, arena.typed_allocator<char>())
            , location(ilocation, arena.typed_allocator<char>())
        {
        }

        char* get_extra()
        {
            return extra_data;
        }

        arena_allocator arena;

        astring name;
        astring location;

        char extra_data[256 - 2 * sizeof(astring) - sizeof(arena_allocator)];
    };

    static_assert(sizeof(network_data) == 256);

    network_data data("Proydakov Evgeny Alexandrovich", "Moscow");
    REQUIRE(data.name == "Proydakov Evgeny Alexandrovich");
    REQUIRE(data.location == "Moscow");

    constexpr size_t data_size = 30;

    REQUIRE(std::string_view(data.extra_data, data_size) == "Proydakov Evgeny Alexandrovich");

    REQUIRE(data.arena.used() >= data_size);
}

TEST_CASE("failed allocation keeps arena state")
{
    alignas(8) char buffer[8] = { };

    // origin is misaligned, so the allocator needs padding before the payload
    arena_allocator arena(buffer + 1, sizeof(buffer) - 3);

    auto typed = arena.typed_allocator<std::uint64_t>();

    REQUIRE(arena.used() == 0);

    // padding jumps past the end of the arena, so the allocation fails
    REQUIRE(typed.allocate(1) == nullptr);
    REQUIRE(arena.used() == 0);
}

TEST_CASE("failed allocation doesn't consume space")
{
    alignas(8) char buffer[8] = { };
    arena_allocator arena(buffer, sizeof(buffer));

    auto typed64 = arena.typed_allocator<std::uint64_t>();
    auto typed8 = arena.typed_allocator<char>();

    REQUIRE(typed64.allocate(2) == nullptr);
    REQUIRE(arena.used() == 0);

    auto p = typed8.allocate(4);
    REQUIRE(p == buffer);
    REQUIRE(arena.used() == 4);
}

TEST_CASE("overflow in element count is rejected")
{
    alignas(8) char buffer[64] = { };
    arena_allocator arena(buffer, sizeof(buffer));

    auto typed = arena.typed_allocator<std::uint16_t>();

    auto const huge = (std::numeric_limits<std::size_t>::max() / sizeof(std::uint16_t)) + 1;

    REQUIRE(typed.allocate(huge) == nullptr);
    REQUIRE(arena.used() == 0);
}
