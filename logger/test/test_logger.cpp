#include <catch2/catch.hpp>

#include <logger/logger.h>
#include <logger/logger_adapter.h>

//
// This test should output:
//
// UTC 2023-06-19 14:54:07.460302 INFO [TEST_MAIN:79325] test_logger.cpp(16):void C_A_T_C_H_T_E_S_T_0() Hello world !!!
//

TEST_CASE("simple_logging")
{
    ihft::logger::logger_adapter::set_thread_name("TEST_MAIN");

    IHFT_LOG_INFO("Hello {} !!!", "world");
}
