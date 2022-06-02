#include <catch2/catch.hpp>

#include <tuple>
#include <sstream>

template<typename Tuple, std::size_t... Is>
void trace_impl(std::ostream& os, Tuple const& t, std::index_sequence<Is...>)
{
    os << "(";
    ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
    os << ")";
}

// see std::apply next time

template<typename Tuple>
void trace(std::ostream& os, Tuple const& t)
{
    constexpr auto tuple_size = std::tuple_size_v<Tuple>;
    using indices = std::make_index_sequence<tuple_size>;
    trace_impl(os, t, indices{});
}

TEST_CASE("compiletime")
{
    auto const t = std::tuple<char, long, double>('a', 1024, 3.14);
    std::stringstream stream;
    trace(stream, t);

    REQUIRE(stream.str() == "(a, 1024, 3.14)");
}
