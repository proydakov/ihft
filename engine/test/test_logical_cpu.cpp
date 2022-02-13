#include <catch2/catch.hpp>

#include <engine/logical_cpu.h>

TEST_CASE("logical_cpu")
{
    ihft::engine::logical_cpu cpu(0, "test");
    REQUIRE(cpu.get_id() == 0);
    REQUIRE(cpu.get_name() == "test");
    REQUIRE(cpu.bind());
}
