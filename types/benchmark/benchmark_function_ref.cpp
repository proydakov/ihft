#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <compiler/compiler.h>
#include <types/function_ref.h>

#include <functional>

namespace
{
    // call types
    using function_ref = ihft::types::function_ref<uint64_t(uint64_t&, uint64_t)>;
    using function_std = std::function<uint64_t(uint64_t&, uint64_t)>;
}

namespace
{
    // simple plain_function

    uint64_t plain_function(uint64_t& origin, uint64_t val)
    {
        origin += val;
        return origin;
    }
}

TEST_CASE("call plain_function benchmark")
{
    uint64_t counter = 0;

    BENCHMARK("plain_function(1)")
    {
        return plain_function(counter, 1);
    };
}

TEST_CASE("call function_ref for plain_function benchmark")
{
    function_ref plaing_function_ref = plain_function;

    uint64_t counter = 0;

    BENCHMARK("function_ref(1)")
    {
        return plaing_function_ref(counter, 1);
    };
}

TEST_CASE("call std_function for plain_function benchmark")
{
    function_std plain_std_function = plain_function;

    uint64_t counter = 0;

    BENCHMARK("std_function(1)")
    {
        return plain_std_function(counter, 1);
    };
}

namespace
{
    // complex lambda function with huge storage content

    constexpr uint64_t v{};

    static auto huge_lambda = [v1 = v, v2 = v, v3 = v, v4 = v, v5 = v, v6 = v, v7 = v, v8 = v]
        (uint64_t& o, uint64_t d) mutable -> uint64_t
    {
        v1 += d;
        v2 += d;
        v3 += d;
        v4 += d;
        v5 += d;
        v6 += d;
        v7 += d;
        v8 += d;

        o += v1 + v2 + v3 + v4 + v5 + v6 + v7 + v8;

        return 0;
    };
}

TEST_CASE("construct + call function_ref for huge_lambda benchmark")
{
    uint64_t counter = 0;

    BENCHMARK("construct function_ref()")
    {
        function_ref plaing_function_ref = huge_lambda;
        return plaing_function_ref(counter, 1);
    };
}

TEST_CASE("construct + call std_function for huge_lambda benchmark")
{
    uint64_t counter = 0;

    BENCHMARK("construct std_function()")
    {
        function_std plain_std_function = huge_lambda;
        return plain_std_function(counter, 1);
    };
}
