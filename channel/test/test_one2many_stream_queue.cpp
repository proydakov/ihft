#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <channel/one2many_stream_pod_queue.h>
#include <channel/one2many_stream_object_queue.h>

#include <thread>

using namespace ihft;

namespace
{
    struct packet
    {
        int header_id{};
        int length{};
    };
}

template<typename Q>
void one2many_stream_queue_simple_methods()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    REQUIRE( queue.size() == qsize );
}

TEST_CASE("one2many_stream_queue simple methods")
{
    one2many_stream_queue_simple_methods<one2many_stream_pod_queue<packet>>();
    one2many_stream_queue_simple_methods<one2many_stream_object_queue<packet>>();
}

template<typename Q>
void one2many_stream_queue_simple_reader()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    REQUIRE( queue.readers_count() == 0 );
    REQUIRE( queue.readers_mask() == 0 );

    // create valid readers
    for(unsigned i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        auto reader_opt = queue.create_reader();
        REQUIRE( reader_opt.has_value() );
        auto& reader = *reader_opt;
        REQUIRE( reader.get_id() == i );
        REQUIRE( queue.readers_count() == i + 1 );
        REQUIRE( queue.readers_mask() != 0 );
    }
}

TEST_CASE("one2many_stream_queue simple reader")
{
    one2many_stream_queue_simple_reader<one2many_stream_pod_queue<packet>>();
    one2many_stream_queue_simple_reader<one2many_stream_object_queue<packet>>();
}

template<typename Q>
void one2many_stream_queue_reader_before_first_write()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    REQUIRE( queue.readers_count() == 0 );
    REQUIRE( queue.readers_mask() == 0 );

    REQUIRE( queue.try_write(packet{}) == true );

    // can't create valid reader
    {
        auto reader_opt = queue.create_reader();
        REQUIRE( not reader_opt.has_value() );
        REQUIRE( queue.readers_count() == 0 );
        REQUIRE( queue.readers_mask() == 0 );
    }
}

TEST_CASE("one2many_stream_queue reader before first write")
{
    one2many_stream_queue_reader_before_first_write<one2many_stream_pod_queue<packet>>();
    one2many_stream_queue_reader_before_first_write<one2many_stream_object_queue<packet>>();
}

template<typename Q>
void one2many_stream_queue_simple_write_and_read()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    auto reader_opt = queue.create_reader();
    REQUIRE( reader_opt.has_value() );
    auto& reader = *reader_opt;

    REQUIRE( not reader.try_read().has_value() );

    for(std::size_t i = 0; i < qsize; i++)
    {
        REQUIRE( queue.try_write(packet{}) == true );
    }

    // queue is full here
    REQUIRE( not queue.try_write(packet{}) );

    for(std::size_t i = 0; i < qsize; i++)
    {
        REQUIRE( reader.try_read().has_value() );
    }

    REQUIRE( not reader.try_read().has_value() );
}

TEST_CASE("one2many_stream_queue simple write + read")
{
    one2many_stream_queue_simple_write_and_read<one2many_stream_pod_queue<packet>>();
    one2many_stream_queue_simple_write_and_read<one2many_stream_object_queue<packet>>();
}

template<typename Q>
void one2many_stream_queue_simple_write_and_part_read()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    auto reader_opt = queue.create_reader();
    REQUIRE( reader_opt.has_value() );
    auto& reader = *reader_opt;

    REQUIRE( not reader.try_read().has_value() );

    for(std::size_t i = 0; i < qsize; i++)
    {
        REQUIRE( queue.try_write(std::make_unique<packet>()) == true );
    }

    // queue is full here
    REQUIRE( not queue.try_write(std::make_unique<packet>()) );

    for(std::size_t i = 0; i < qsize / 2; i++)
    {
        REQUIRE( reader.try_read().has_value() );
    }

    // no leaks should be here
}

TEST_CASE("one2many_stream_queue simple write + part read")
{
    one2many_stream_queue_simple_write_and_part_read<one2many_stream_object_queue<std::unique_ptr<packet>>>();
}
