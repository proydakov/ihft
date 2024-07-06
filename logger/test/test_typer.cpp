#include <catch2/catch_test_macros.hpp>

#include <tuple>
#include <string>
#include <string_view>
#include <type_traits>

template<typename T>
struct typer
{
    using type = void;
};

template<typename T>
struct typer<T*>
{
    using type = void*;
};

template<typename T>
struct typer<T* const>
{
    using type = void*;
};

template<>
struct typer<nullptr_t>
{
    using type = void*;
};

template<>
struct typer<int>
{
    using type = long;
};

template<>
struct typer<float>
{
    using type = double;
};

template<>
struct typer<std::string>
{
    using type = std::string_view;
};

template<typename ... Args>
decltype(auto) to_tuple(Args&& ...)
{
    return std::tuple<typename typer<Args>::type ...>();
}

TEST_CASE("typer")
{
    char* ptr0 = nullptr;
    const char* ptr1 = nullptr;
    char* const ptr2 = nullptr;
    const char* const ptr3 = nullptr;

    static_assert(std::is_same_v<typer<decltype(ptr0)>::type, void*>);
    static_assert(std::is_same_v<typer<decltype(ptr1)>::type, void*>);
    static_assert(std::is_same_v<typer<decltype(ptr2)>::type, void*>);
    static_assert(std::is_same_v<typer<decltype(ptr3)>::type, void*>);
    static_assert(std::is_same_v<typer<nullptr_t>::type, void*>);
    static_assert(std::is_same_v<std::tuple<long, double, std::string_view>, decltype(to_tuple(int{}, float{}, std::string{}))>);
}
