#include <catch2/catch.hpp>

#include <array>
#include <tuple>
#include <sstream>
#include <string_view>

// Formating code

template<typename Tuple, typename Array, std::size_t... Is>
void format_impl(std::ostream& os, Array const& a, Tuple const& t, std::index_sequence<Is...>)
{
    ((os << a[Is] << std::get<Is>(t)), ...);
    os << a.back();
}

template<typename Tuple>
void format(std::ostream& os, std::string_view expr, Tuple const& t)
{
    constexpr auto tuple_size = std::tuple_size_v<Tuple>;
    constexpr auto array_size = tuple_size + 1; // need extra place for tail

    std::array<std::string_view, array_size> array;
    size_t from{}, count{}, pos{};

    while(pos = expr.find("{}", from), pos != std::string_view::npos and count < tuple_size)
    {
        array[count] = expr.substr(from, pos - from);

        count += 1;
        pos += 2;
        from = pos;
    }

    array[count] = expr.substr(from);

    using indices = std::make_index_sequence<tuple_size>;
    format_impl(os, array, t, indices{});
}

// Unit tests

TEST_CASE("format success 1")
{
    auto const t = std::tuple<char, long, double>('a', 1024, 3.14);
    std::string_view const expr = "arg1: '{}', arg2: {}, arg3: {}f";

    std::ostringstream stream;
    format(stream, expr, t);

    REQUIRE(stream.str() == "arg1: 'a', arg2: 1024, arg3: 3.14f");
}

TEST_CASE("format success 2")
{
    auto const t = std::tuple<char, long, double>('a', 1024, 3.14);
    std::string_view const expr = "arg1: '{}', arg2: {}, arg3: ";

    std::ostringstream stream;
    format(stream, expr, t);

    REQUIRE(stream.str() == "arg1: 'a', arg2: 1024, arg3: 3.14");
}

TEST_CASE("format success 3")
{
    auto const t = std::tuple<char, long, double, char>('a', 1024, 3.14, 'e');
    std::string_view const expr = "arg1: '{}', arg2: {}, arg3: ";

    std::ostringstream stream;
    format(stream, expr, t);

    REQUIRE(stream.str() == "arg1: 'a', arg2: 1024, arg3: 3.14e");
}

TEST_CASE("format success 4")
{
    auto const t = std::tuple<char, double, char>('a', 3.14, 'e');
    std::string_view const expr = "arg1: '{}', arg2: {}, arg3: '{}', arg4: {}, arg5: '{}'";

    std::ostringstream stream;
    format(stream, expr, t);

    REQUIRE(stream.str() == "arg1: 'a', arg2: 3.14, arg3: 'e', arg4: {}, arg5: '{}'");
}
