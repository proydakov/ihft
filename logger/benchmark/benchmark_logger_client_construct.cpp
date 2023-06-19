#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <logger/logger_event.h>
#include <logger/logger_client.h>

#include <cstring>

using namespace ihft::logger;

TEST_CASE("log event serrialize")
{
    auto client = logger_client::get_this_thread_client();

    REQUIRE( client );

    auto event_slab = client->active_event_slab();

    const char * const tname = "main";
    char array[16];
    strncpy(array, tname, strnlen(tname, sizeof(array)));

    BENCHMARK("log_event(C++, IHFT, 1024ul)")
    {
        auto event = std::construct_at(event_slab,
            "benchmark constexpr args: {} {} {}",
            "C++",
            "IHFT",
            1024ul
        );
        event->set_log_point_info( log_level::INFO, std::chrono::system_clock::now(), source_location_current( ) );
        event->set_thread_info(256, array);

        std::destroy_at(event);
    };
}
