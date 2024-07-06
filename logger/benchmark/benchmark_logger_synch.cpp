#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <logger/logger_event.h>
#include <logger/logger_client.h>
#include <logger/logger_adapter.h>
#include <logger/logger_listener.h>

using namespace ihft::logger;

namespace
{
    class benchmark_logger_listener final : public logger_listener
    {
    public:
        void notify(std::string_view) override
        {
        }

        void flush() override
        {
        }
    };
}

TEST_CASE("log event benchmark")
{
    {
        auto uniq = std::make_unique<benchmark_logger_listener>();

        REQUIRE( uniq );

        logger_adapter::replace_listener(std::move(uniq));
    }

    auto client = logger_client::get_this_thread_client();

    REQUIRE( client );

    BENCHMARK("log_event(C++, IHFT, 1024ul)")
    {
        auto event_slab = client->active_event_slab();

        auto event = std::construct_at(event_slab,
            "benchmark constexpr args: {} {} {}",
            "C++",
            "IHFT",
            1024ul
        );

        return client->try_log_event(event);
    };
}
