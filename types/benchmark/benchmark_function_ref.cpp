#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <types/function_ref.h>

#include <functional>

std::size_t plain_function(std::size_t& origin, std::size_t val)
{
    origin += val;
    return origin;
}

TEST_CASE("plain_function benchmark")
{
    std::size_t counter = 0;

    BENCHMARK("plain_function(1)")
    {
        return plain_function(counter, 1);
    };
}

TEST_CASE("function_ref for plain_function benchmark")
{
    using function_ref = ihft::types::function_ref<std::size_t(std::size_t&, std::size_t)>;
    function_ref plaing_function_ref = function_ref::function<&plain_function>();

    std::size_t counter = 0;

    BENCHMARK("function_ref(1)")
    {
        return plaing_function_ref(counter, 1);
    };
}

TEST_CASE("std_function for plain_function benchmark")
{
    using function = std::function<std::size_t(std::size_t&, std::size_t)>;
    function plain_function_stl = plain_function;

    std::size_t counter = 0;

    BENCHMARK("std_function(1)")
    {
        return plain_function_stl(counter, 1);
    };
}
