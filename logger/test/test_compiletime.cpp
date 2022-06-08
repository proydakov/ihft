#include <catch2/catch.hpp>

#include <string_view>

consteval size_t compiletime_count(std::string_view view, std::string_view pattern)
{
    size_t res{};
    size_t pos{};
    while(pos = view.find(pattern, pos), pos != std::string_view::npos)
    {
        res += 1;
        pos += pattern.size();
    }
    return res;
}

template<typename ... Args>
consteval size_t compiletime_size(Args&& ... args)
{
    return sizeof...(args);
}

#define CHECK_ME(pattern, ...) \
do { \
    static_assert(compiletime_count(pattern, "{}") == compiletime_size(__VA_ARGS__)); \
    /* this should be the second function call */ \
} \
while(0)

#include <string>

TEST_CASE("compiletime")
{
    using namespace std::literals;

    static_assert(compiletime_count("hello: {} world: {}"sv, "{}") == 2);
    static_assert(compiletime_count("{}"sv, "{}") == 1);
    static_assert(compiletime_count("simple str"sv, "{}") == 0);
    static_assert(compiletime_count("{} {} {} {}"sv, "{}") == 4);

    static_assert(compiletime_size() == 0);
    static_assert(compiletime_size(1) == 1);
    static_assert(compiletime_size(1, 2) == 2);
    static_assert(compiletime_size(1, 2.0, 'a') == 3);
    static_assert(compiletime_size(1, 2.0, 'a', "hello"sv) == 4);

    CHECK_ME("no items here", );
    CHECK_ME("hello: {}, world: {}", 'a', 1);
    CHECK_ME("{} - {} - {}", "this", "world", "is huge");

    // This is doesn't work because it is not consteval expression
    //std::string str("{}");
    //CHECK_ME(str, 1);
}
