#include <catch2/catch_test_macros.hpp>

#include <channel/channel_factory.h>
#include <channel/one2one_seqnum_stream_pod_queue.h>
#include <channel/one2one_seqnum_stream_object_queue.h>
#include <channel/one2each_seqnum_stream_pod_queue.h>
#include <channel/one2each_seqnum_stream_object_queue.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <thread>

using namespace ihft::channel;

namespace
{
    // Minimal pooled allocator: only capacity() is needed to exercise the
    // channel_factory capacity check.
    struct dummy_pool_allocator final
    {
        explicit dummy_pool_allocator(std::size_t capacity) noexcept
            : m_capacity(capacity)
        {
        }

        std::size_t capacity() const noexcept
        {
            return m_capacity;
        }

        std::size_t m_capacity;
    };

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

    auto opt = channel_factory::make<Q>(qsize, 1);

    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;

    REQUIRE( queue.capacity() == qsize );
}

TEST_CASE("one2*_stream_queue simple methods")
{
    one2_stream_queue_simple_methods<one2one_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_simple_methods<one2one_seqnum_stream_object_queue<packet_t>>();
    one2_stream_queue_simple_methods<one2each_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_simple_methods<one2each_seqnum_stream_object_queue<packet_t>>();
}

template<typename Q>
void one2_stream_queue_min_capacity()
{
    constexpr std::size_t qsize = 0;

    auto opt = channel_factory::make<Q>(qsize, 1);

    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;

    REQUIRE( queue.capacity() == 4 );
}

TEST_CASE("one2*_stream_queue min capacity")
{
    one2_stream_queue_min_capacity<one2one_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_min_capacity<one2one_seqnum_stream_object_queue<packet_t>>();
    one2_stream_queue_min_capacity<one2each_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_min_capacity<one2each_seqnum_stream_object_queue<packet_t>>();
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
void one2each_stream_queue_simple_reader()
{
    constexpr std::size_t qsize = 32;

    auto const cpus = std::min<std::size_t>(std::thread::hardware_concurrency(), Q::MAX_READERS);

    auto opt = channel_factory::make<Q>(qsize, cpus);
    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;
    auto& readers = opt->consumers;

    REQUIRE( queue.readers_count() == cpus );

    // create valid readers
    for(std::size_t i = 0; i < cpus; i++)
    {
        REQUIRE( readers[i].get_id() == i );
    }
}

TEST_CASE("one2each_stream_queue simple reader")
{
    one2each_stream_queue_simple_reader<one2each_seqnum_stream_pod_queue<packet_t>>();
    one2each_stream_queue_simple_reader<one2each_seqnum_stream_object_queue<packet_t>>();
}

TEST_CASE("one2each queue limits the number of readers")
{
    using queue_t = one2each_seqnum_stream_pod_queue<packet_t>;

    constexpr std::size_t max_readers = queue_t::MAX_READERS;

    auto const max_ok = channel_factory::make<queue_t>(32, max_readers);
    REQUIRE( max_ok.has_value() );
    REQUIRE( max_ok->producer.readers_count() == max_readers );
    REQUIRE( max_ok->producer.readers_mask() == std::numeric_limits<std::size_t>::max() );

    auto const too_many = channel_factory::make<queue_t>(32, max_readers + 1);
    REQUIRE_FALSE( too_many.has_value() );
}

TEST_CASE("one2each object queue honours the reader limit")
{
    using queue_t = one2each_seqnum_stream_object_queue<std::size_t, impl::empty_allocator, std::uint8_t>;

    constexpr std::size_t max_readers = queue_t::MAX_READERS;

    auto const max_ok = channel_factory::make<queue_t>(32, max_readers);
    REQUIRE( max_ok.has_value() );
    REQUIRE( max_ok->producer.readers_count() == max_readers );

    auto const too_many = channel_factory::make<queue_t>(32, max_readers + 1);
    REQUIRE_FALSE( too_many.has_value() );
}

TEST_CASE("content allocator must cover the rounded queue capacity")
{
    using allocator_t = dummy_pool_allocator;
    using queue_t = one2one_seqnum_stream_object_queue<std::size_t, allocator_t>;

    // 1000 rounds up to 1024; a pool of 1001 slots is not enough.
    auto const too_small = channel_factory::make<queue_t>(1000, 1, std::make_unique<allocator_t>(1001));
    REQUIRE_FALSE( too_small.has_value() );

    // One spare slot beyond the rounded capacity is enough.
    auto const exact = channel_factory::make<queue_t>(1024, 1, std::make_unique<allocator_t>(1025));
    REQUIRE( exact.has_value() );
    REQUIRE( exact->producer.capacity() == 1024 );
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
    one2_stream_queue_simple_write_and_read<one2each_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_simple_write_and_read<one2each_seqnum_stream_object_queue<packet_t>>();
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
    one2_stream_queue_simple_write_and_part_read<one2each_seqnum_stream_object_queue<std::unique_ptr<packet_t>>>();
}

template<typename Q>
void one2_stream_queue_complex_write_and_each_read_attemps()
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

TEST_CASE("one2*_stream_queue complex write + each read attemps")
{
    one2_stream_queue_complex_write_and_each_read_attemps<one2one_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_complex_write_and_each_read_attemps<one2one_seqnum_stream_object_queue<packet_t>>();
    one2_stream_queue_complex_write_and_each_read_attemps<one2each_seqnum_stream_pod_queue<packet_t>>();
    one2_stream_queue_complex_write_and_each_read_attemps<one2each_seqnum_stream_object_queue<packet_t>>();
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

    for(std::size_t i = 0; i < std::size_t(std::numeric_limits<uint8_t>::max()) * 7; i++)
    {
        REQUIRE( not reader.try_read().has_value() );
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
    one2_stream_queue_stress_write_and_read<one2one_seqnum_stream_object_queue<std::size_t, ihft::channel::impl::empty_allocator, std::uint8_t>>();
    one2_stream_queue_stress_write_and_read<one2one_seqnum_stream_pod_queue<std::size_t, std::uint8_t>>();
    one2_stream_queue_stress_write_and_read<one2one_seqnum_stream_object_queue<std::size_t, ihft::channel::impl::empty_allocator, std::uint8_t>>();
}

/*

// The test case for guard queue and cyclic read issue.
// We used 1/4 from sequence type max value for queue size to avoid this problem.

template<typename Q>
void one2_stream_guard_queue_overflow()
{
    constexpr std::size_t qsize = 128;

    auto opt = channel_factory::make<Q>(qsize, 1);
    REQUIRE( opt.has_value() );

    auto& queue = opt->producer;
    auto& reader = opt->consumers.back();

    REQUIRE( queue.capacity() == qsize );

    REQUIRE( not reader.try_read().has_value() );

    std::vector<typename Q::reader_type::guard_type> vector;

    for(std::size_t i = 0; i <= std::size_t(std::numeric_limits<uint8_t>::max()) / 2; i++)
    {
        REQUIRE( not reader.try_read().has_value() );
        REQUIRE( queue.try_write(std::size_t(i)) == true );
        auto opt = reader.try_read();
        REQUIRE( opt.has_value() );
        std::size_t const& val = *opt;
        REQUIRE(val == i);
        vector.push_back(std::move(*opt));
    }

    {
        // check max -> zero overflow + read next sequence
        REQUIRE( not reader.try_read().has_value() );
    }
}

TEST_CASE("one2_stream_guard_queue_cycle_seqnum_overflow")
{
    one2_stream_guard_queue_overflow<one2one_seqnum_stream_object_queue<std::size_t, ihft::channel::impl::empty_allocator, std::uint8_t>>();
    one2_stream_guard_queue_overflow<one2each_seqnum_stream_object_queue<std::size_t, ihft::channel::impl::empty_allocator, std::uint8_t>>();
}
*/
