#include <catch2/catch_test_macros.hpp>

#include <logger/logger.h>

//
// This test should output:
//
// UTC 2023-06-19 14:54:07.460302 INFO [main:79325] test_logger.cpp(16):void C_A_T_C_H_T_E_S_T_0() Hello world !!!
//

TEST_CASE("simple_logging")
{
    IHFT_LOG_INFO("Hello {} !!!", "world");
}
