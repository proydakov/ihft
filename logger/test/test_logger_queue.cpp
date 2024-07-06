#include <catch2/catch_test_macros.hpp>

#include <logger/logger_event.h>

#include <memory/stream_fixed_pool_allocator.h>

#include <channel/channel_factory.h>
#include <channel/one2one_seqnum_stream_object_queue.h>

#include <types/box.h>

#include <sstream>

using namespace ihft;

TEST_CASE("try to queue 5 events")
{
    constexpr size_t QUEUE_SIZE = 4;

    using event_t = types::box<logger::logger_event>;
    using alloc_t = memory::stream_fixed_pool_allocator<logger::logger_event>;
    using queue_t = channel::one2one_seqnum_stream_object_queue<event_t, alloc_t>;

    auto opt = channel::channel_factory::make<queue_t>(QUEUE_SIZE, 1, std::make_unique<alloc_t>(QUEUE_SIZE));

    REQUIRE(opt);

    auto& producer = opt->producer;
    auto& consumer = opt->consumers.front();
    auto& allocator = producer.get_content_allocator();

    for(size_t i = 0; i < QUEUE_SIZE; i++)
    {
        REQUIRE(allocator.position() == i);
        auto event = std::construct_at(allocator.active_slab(), "args constexpr: {} {}", "C++", "IHFT");
        REQUIRE(producer.try_write(types::box<logger::logger_event>(event)));
        allocator.seek_to_next_slab();
        REQUIRE(allocator.position() == (i + 1));
    }

    {
        REQUIRE(allocator.position() == 4);
        auto event = std::construct_at(allocator.active_slab(), "invalid args constexpr");
        REQUIRE(not producer.try_write(types::box<logger::logger_event>(event)));
        REQUIRE(allocator.position() == 4);
    }

    {
        auto opt = consumer.try_read();
        REQUIRE(opt);

        logger::logger_event const& event_ref = opt->get_event();

        std::ostringstream sstream;
        event_ref.print_args_to(sstream);

        REQUIRE(sstream.str() == "args constexpr: C++ IHFT");
    }
}
