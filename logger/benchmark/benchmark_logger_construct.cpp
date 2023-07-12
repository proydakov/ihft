/*

-------------------------------------------------------------------------------
log event serrialize
-------------------------------------------------------------------------------
../logger/benchmark/benchmark_logger_construct.cpp:12
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
log_event(C++, IHFT, 1024ul)                   100           866     2.0784 ms
                                        24.4293 ns    24.4215 ns    24.4601 ns
                                      0.0729872 ns 0.00705451 ns   0.173306 ns

===============================================================================
All tests passed (1 assertion in 1 test case)

*/

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <logger/logger_event.h>
#include <logger/logger_client.h>
#include <platform/platform.h>

#include <cstring>

using namespace ihft::logger;

TEST_CASE("log event serrialize")
{
    auto client = logger_client::get_this_thread_client();

    REQUIRE( client );

    auto event_slab = client->active_event_slab();

    const long tid = ihft::platform::trait::get_thread_id();

    const char * const tname = "main";
    char array[16] = {'\0'};
    strncpy(array, tname, strnlen(tname, sizeof(array)));

    BENCHMARK("log_event(C++, IHFT, 1024ul)")
    {
        auto event = std::construct_at(event_slab,
            "benchmark constexpr args: {} {} {}",
            "C++",
            "IHFT",
            1024ul
        );
        event->set_log_point_source_info(log_level::INFO, logger_event::clock_t::now(), source_location_current());
        event->set_log_point_thread_info(tid, array);

        std::destroy_at(event);
    };
}
