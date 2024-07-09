#include <catch2/catch_test_macros.hpp>

#include <types/scope_exit.h>

using namespace ihft::types;

TEST_CASE("scope_exit")
{
    int check = 0;

    {
        auto exit = make_scope_exit([&]() {
            check = 2024;
        });
    }

    REQUIRE(2024 == check);
}

TEST_CASE("scope_exit_move")
{
    int check = 0;

    {
        auto exit = make_scope_exit([&]() {
            check = 2024;
        });

        auto next = std::move(exit);

        REQUIRE(0 == check);

        exit.release();

        REQUIRE(0 == check);
    }

    REQUIRE(2024 == check);
}
