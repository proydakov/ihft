#include "catch2/catch.hpp"

#include <channel/one2one_seqnum_stream_pod_queue.h>
#include <channel/one2one_seqnum_stream_object_queue.h>
#include <channel/one2many_seqnum_stream_pod_queue.h>
#include <channel/one2many_seqnum_stream_object_queue.h>

#include <limits>
#include <thread>

using namespace ihft;

namespace
{
    struct packet_t final
    {
        std::size_t header_id{};
        std::size_t length{};
    };

    bool operator==(const packet_t& p1, const packet_t& p2) noexcept
    {
        return p1.header_id == p2.header_id && p1.length == p2.length;
    }
}

template<typename Q>
void one2_stream_queue_simple_methods()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    REQUIRE( queue.capacity() == qsize );
}

TEST_CASE("one2*_stream_queue simple methods")
{
    one2_stream_queue_simple_methods<one2one_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_simple_methods<one2one_seqnum_stream_object_queue<packet_t>>();
    one2_stream_queue_simple_methods<one2many_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_simple_methods<one2many_seqnum_stream_object_queue<packet_t>>();
}

template<typename Q>
void one2_stream_queue_min_capacity()
{
    constexpr std::size_t qsize = 0;

    Q queue(qsize);

    REQUIRE( queue.capacity() == 2 );
}

TEST_CASE("one2*_stream_queue min capacity")
{
    one2_stream_queue_min_capacity<one2one_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_min_capacity<one2one_seqnum_stream_object_queue<packet_t>>();
    one2_stream_queue_min_capacity<one2many_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_min_capacity<one2many_seqnum_stream_object_queue<packet_t>>();
}

template<typename Q>
void one2one_stream_queue_simple_reader()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    REQUIRE( queue.readers_count() == 0 );
    REQUIRE( queue.readers_mask() == 0 );

    // create valid reader
    {
        auto reader_opt = queue.create_reader();
        REQUIRE( reader_opt.has_value() );
        auto& reader = *reader_opt;
        REQUIRE( reader.get_id() == 0 );
        REQUIRE( queue.readers_count() == 1 );
        REQUIRE( queue.readers_mask() == 1 );
    }

    // one2one queue support only a single reader
    {
        auto reader_opt = queue.create_reader();
        REQUIRE( not reader_opt.has_value() );
        REQUIRE( queue.readers_count() == 1 );
        REQUIRE( queue.readers_mask() == 1 );
    }
}

TEST_CASE("one2one_stream_queue simple reader")
{
    one2one_stream_queue_simple_reader<one2one_seqnum_stream_pod_queue<packet_t>>();
    one2one_stream_queue_simple_reader<one2one_seqnum_stream_object_queue<packet_t>>();
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
    one2many_stream_queue_simple_reader<one2many_seqnum_stream_pod_queue<packet_t>>();
    one2many_stream_queue_simple_reader<one2many_seqnum_stream_object_queue<packet_t>>();
}

template<typename Q>
void one2_stream_queue_reader_before_first_write()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    REQUIRE( queue.readers_count() == 0 );
    REQUIRE( queue.readers_mask() == 0 );

    REQUIRE( queue.try_write(packet_t{}) == true );

    // can't create valid reader
    {
        auto reader_opt = queue.create_reader();
        REQUIRE( not reader_opt.has_value() );
        REQUIRE( queue.readers_count() == 0 );
        REQUIRE( queue.readers_mask() == 0 );
    }
}

TEST_CASE("one2*_stream_queue reader before first write")
{
    one2_stream_queue_reader_before_first_write<one2one_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_reader_before_first_write<one2one_seqnum_stream_object_queue<packet_t>>();
    one2_stream_queue_reader_before_first_write<one2many_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_reader_before_first_write<one2many_seqnum_stream_object_queue<packet_t>>();
}

template<typename Q>
void one2_stream_queue_simple_write_and_read()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    auto reader_opt = queue.create_reader();
    REQUIRE( reader_opt.has_value() );
    auto& reader = *reader_opt;

    REQUIRE( not reader.try_read().has_value() );

    for(std::size_t i = 0; i < qsize; i++)
    {
        REQUIRE( queue.try_write(packet_t{i, 0}) == true );
    }

    // queue is full here
    REQUIRE( not queue.try_write(packet_t{1024, 0}) );

    for(std::size_t i = 0; i < qsize; i++)
    {
        auto const opt = reader.try_read();
        REQUIRE( opt.has_value() );
        auto const& packet = *opt;
        REQUIRE( packet == packet_t{i, 0} );
    }

    REQUIRE( not reader.try_read().has_value() );
}

TEST_CASE("one2*_stream_queue simple write + read")
{
    one2_stream_queue_simple_write_and_read<one2one_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_simple_write_and_read<one2one_seqnum_stream_object_queue<packet_t>>();
    one2_stream_queue_simple_write_and_read<one2many_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_simple_write_and_read<one2many_seqnum_stream_object_queue<packet_t>>();
}

template<typename Q>
void one2_stream_queue_simple_write_and_part_read()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    auto reader_opt = queue.create_reader();
    REQUIRE( reader_opt.has_value() );
    auto& reader = *reader_opt;

    REQUIRE( not reader.try_read().has_value() );

    for(std::size_t i = 0; i < qsize; i++)
    {
        REQUIRE( queue.try_write(std::make_unique<packet_t>()) == true );
    }

    // queue is full here
    REQUIRE( not queue.try_write(std::make_unique<packet_t>()) );

    for(std::size_t i = 0; i < qsize / 2; i++)
    {
        REQUIRE( reader.try_read().has_value() );
    }

    // no leaks should be here
}

TEST_CASE("one2*_stream_queue simple write + part read")
{
    one2_stream_queue_simple_write_and_part_read<one2one_seqnum_stream_object_queue<std::unique_ptr<packet_t>>>();
    one2_stream_queue_simple_write_and_part_read<one2many_seqnum_stream_object_queue<std::unique_ptr<packet_t>>>();
}

template<typename Q>
void one2_stream_queue_complex_write_and_many_read_attemps()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    auto reader_opt = queue.create_reader();
    REQUIRE( reader_opt.has_value() );
    auto& reader = *reader_opt;

    for(std::size_t i = 0; i < qsize; i++)
    {
        REQUIRE( queue.try_write(packet_t{i, i}) == true );

        auto const opt = reader.try_read();
        REQUIRE( opt.has_value() );
        auto const& packet = *opt;
        REQUIRE( packet == packet_t{i, i} );

        for(int j = 0; j < 3; j++)
        {
            REQUIRE( not reader.try_read().has_value() );
        }
    }
}

TEST_CASE("one2*_stream_queue complex write + many read attemps")
{
    one2_stream_queue_complex_write_and_many_read_attemps<one2one_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_complex_write_and_many_read_attemps<one2one_seqnum_stream_object_queue<packet_t>>();
    one2_stream_queue_complex_write_and_many_read_attemps<one2many_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_complex_write_and_many_read_attemps<one2many_seqnum_stream_object_queue<packet_t>>();
}

template<typename Q>
void one2_stream_queue_stress_write_and_read()
{
    constexpr std::size_t qsize = 32;

    Q queue(qsize);

    auto reader_opt = queue.create_reader();
    REQUIRE( reader_opt.has_value() );
    auto& reader = *reader_opt;

    REQUIRE( not reader.try_read().has_value() );

    for(std::size_t i = 0; i < std::size_t(std::numeric_limits<uint8_t>::max()) * 3; i++)
    {
        REQUIRE( queue.try_write(std::size_t(i)) == true );
        auto opt = reader.try_read();
        REQUIRE( opt.has_value() );
        std::size_t const& val = *opt;
        REQUIRE(val == i);
    }
}

TEST_CASE("one2*_stream_queue stress write + read")
{
    one2_stream_queue_stress_write_and_read<one2one_seqnum_stream_pod_queue<std::size_t, std::uint8_t>>();
    one2_stream_queue_stress_write_and_read<one2one_seqnum_stream_object_queue<std::size_t, ihft::channel::empty_allocator, std::uint8_t>>();
    one2_stream_queue_stress_write_and_read<one2one_seqnum_stream_pod_queue<std::size_t, std::uint8_t>>();
    one2_stream_queue_stress_write_and_read<one2one_seqnum_stream_object_queue<std::size_t, ihft::channel::empty_allocator, std::uint8_t>>();
}
