#include <catch2/catch.hpp>

#include <channel/channel_factory.h>
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

    [[maybe_unused]] bool operator==(const packet_t& p1, const packet_t& p2) noexcept
    {
        return p1.header_id == p2.header_id && p1.length == p2.length;
    }
}

template<typename Q>
void one2_stream_queue_simple_methods()
{
    constexpr std::size_t qsize = 32;

    auto opt = channel_factory::make<Q>(qsize, 0);

    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;

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

    auto opt = channel_factory::make<Q>(qsize, 0);

    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;

    REQUIRE( queue.capacity() == 4 );
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

    {
        auto opt = channel_factory::make<Q>(qsize, 1);
        REQUIRE( opt.has_value() );
    }

    {
        auto opt = channel_factory::make<Q>(qsize, 2);
        REQUIRE( not opt.has_value() );
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

    auto const cpus = std::thread::hardware_concurrency();

    auto opt = channel_factory::make<Q>(qsize, cpus);
    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;
    auto& readers = opt->consumers;

    REQUIRE( queue.readers_count() == cpus );

    // create valid readers
    for(unsigned i = 0; i < cpus; i++)
    {
        REQUIRE( readers[i].get_id() == i );
    }
}

TEST_CASE("one2many_stream_queue simple reader")
{
    one2many_stream_queue_simple_reader<one2many_seqnum_stream_pod_queue<packet_t>>();
    one2many_stream_queue_simple_reader<one2many_seqnum_stream_object_queue<packet_t>>();
}

template<typename Q>
void one2_stream_queue_simple_write_and_read()
{
    constexpr std::size_t qsize = 32;

    auto opt = channel_factory::make<Q>(qsize, 1);
    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;
    auto& reader = opt->consumers.back();

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

    auto opt = channel_factory::make<Q>(qsize, 1);
    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;
    auto& reader = opt->consumers.back();

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

    auto opt = channel_factory::make<Q>(qsize, 1);
    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;
    auto& reader = opt->consumers.back();

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

    auto opt = channel_factory::make<Q>(qsize, 1);
    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;
    auto& reader = opt->consumers.back();

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

template<typename Q>
void one2one_stream_queue_overflow()
{
    constexpr std::size_t qsize = 32;

    auto opt = channel_factory::make<Q>(qsize, 1);
    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;
    auto& reader = opt->consumers.back();

    REQUIRE( not reader.try_read().has_value() );

    for(std::size_t i = 0; i < std::size_t(std::numeric_limits<uint8_t>::max()); i++)
    {
        REQUIRE( queue.try_write(std::size_t(i)) == true );
        auto opt = reader.try_read();
        REQUIRE( opt.has_value() );
    }

    {
        // check max -> zero overflow + read next sequence
        REQUIRE( not reader.try_read().has_value() );
    }
}

TEST_CASE("one2one_stream_queue_cycle_seqnum_overflow")
{
    one2one_stream_queue_overflow<one2one_seqnum_stream_pod_queue<std::size_t, std::uint8_t>>();
    one2one_stream_queue_overflow<one2one_seqnum_stream_object_queue<std::size_t, ihft::channel::empty_allocator, std::uint8_t>>();
}
