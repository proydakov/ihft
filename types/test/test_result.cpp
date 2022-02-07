#include "catch2/catch.hpp"

#include "types/result.h"

using namespace ihft::types;

TEST_CASE("succeeded")
{
    constexpr int VALUE = 1024;

    result<int, std::string> res(VALUE);

    REQUIRE(res);

    REQUIRE(res.succeeded());

    REQUIRE(res.value() == VALUE);
}

TEST_CASE("failed")
{
    const std::string ERROR = "Invalid output";

    result<int, std::string> res(ERROR);

    REQUIRE(!res);

    REQUIRE(res.failed());

    REQUIRE(res.error() == ERROR);
}
