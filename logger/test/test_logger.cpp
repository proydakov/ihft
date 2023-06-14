#include <catch2/catch.hpp>

#include <logger/logger.h>

//
// This test should output:
//
// UTC 2023-06-14 16:28:26.457495 INFO test_logger.cpp(13):void C_A_T_C_H_T_E_S_T_0() Hello world !!!
//

TEST_CASE("simple_logging")
{
    IHFT_LOG_INFO("Hello {} !!!", "world");
}
