#include <catch2/catch_test_macros.hpp>

#include <logger/logger_event.h>
#include <logger/logger_client.h>
#include <logger/logger_adapter.h>
#include <logger/logger_listener.h>

using namespace ihft::logger;

namespace
{
    class test_logger_listener final : public logger_listener
    {
    public:
        test_logger_listener()
            : m_cnt()
        {
        }

        void notify(std::string_view view) override
        {
            m_cnt++;
            m_last = view;
        }

        void flush() override
        {
        }

        std::size_t m_cnt;
        std::string m_last;
    };
}

TEST_CASE("log simple event")
{
    auto uniq = std::make_shared<test_logger_listener>();

    REQUIRE( uniq );

    auto& listener = *uniq.get();

    logger_adapter::replace_listener(std::move(uniq));

    REQUIRE( logger_client::get_this_thread_client() );

    auto client = logger_client::get_this_thread_client();

    REQUIRE( client );

    for(size_t i = 1; i <= 3; i++)
    {
        auto event_slab = client->active_event_slab();

        auto event = std::construct_at(event_slab, "args constexpr: {} {} iter: {}", "C++", "IHFT", i);

        REQUIRE( listener.m_cnt == i - 1 );

        REQUIRE( client->try_log_event(event) );

        REQUIRE( listener.m_cnt == i );
        REQUIRE( listener.m_last == "args constexpr: C++ IHFT iter: " + std::to_string(i));
    }
}
