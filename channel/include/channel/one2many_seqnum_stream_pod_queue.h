#pragma once

#include "private/one2many_seqnum_stream_queue_impl.h"

#include <atomic>
#include <memory>
#include <optional>
#include <type_traits>

namespace ihft
{

// predeclaration

// reader
template<class event_t, typename counter_t>
class one2many_seqnum_stream_pod_reader;

// queue
template<class event_t, typename counter_t>
class one2many_seqnum_stream_pod_queue;

// buffer
template<class event_t, typename counter_t>
using one2many_seqnum_stream_ring_buffer_t = std::shared_ptr<channel::one2many_seqnum_bucket<event_t, counter_t>>;

// implementation

// reader
template<class event_t, typename counter_t>
class alignas(channel::CPU_CACHE_LINE_SIZE) one2many_seqnum_stream_pod_reader final
{
public:
    using ring_buffer_t = one2many_seqnum_stream_ring_buffer_t<event_t, counter_t>;
    using event_type = event_t;

public:
    one2many_seqnum_stream_pod_reader(one2many_seqnum_stream_pod_reader&&) noexcept = default;

    one2many_seqnum_stream_pod_reader& operator=(one2many_seqnum_stream_pod_reader&&) noexcept = delete;
    one2many_seqnum_stream_pod_reader(const one2many_seqnum_stream_pod_reader&) = delete;
    one2many_seqnum_stream_pod_reader& operator=(const one2many_seqnum_stream_pod_reader&) = delete;

    std::optional<event_t> try_read() noexcept
    {
        auto& bucket = m_storage.get()[m_next_bucket];
        if (bucket.m_seqn.load(std::memory_order_acquire) == m_next_read_index)
        {
            m_next_read_index++;
            m_next_bucket = m_next_read_index & m_storage_mask;
            std::optional<event_t> opt(std::in_place, bucket.get_event());
            bucket.m_counter.fetch_sub(1, std::memory_order_release);
            return opt;
        }
        else
        {
            return std::nullopt;
        }
    }

    counter_t get_id() const noexcept
    {
        return m_id;
    }

private:
    one2many_seqnum_stream_pod_reader(ring_buffer_t storage, std::size_t storage_mask, counter_t read_from, counter_t id) noexcept
        : m_storage(std::move(storage))
        , m_next_bucket(read_from & storage_mask)
        , m_storage_mask(storage_mask)
        , m_next_read_index(read_from)
        , m_id(id)
    {
        static_assert(sizeof(one2many_seqnum_stream_pod_reader<event_t, counter_t>) <= channel::CPU_CACHE_LINE_SIZE);
    }

private:
    friend class channel::one2many_seqnum_stream_queue_impl<event_t, counter_t>;

    ring_buffer_t m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_read_index;
    counter_t m_id;
};

// queue
template<class event_t, typename counter_t = std::uint32_t>
class alignas(channel::CPU_CACHE_LINE_SIZE) one2many_seqnum_stream_pod_queue final
{
public:
    using reader_type = one2many_seqnum_stream_pod_reader<event_t, counter_t>;
    using ring_buffer_t = one2many_seqnum_stream_ring_buffer_t<event_t, counter_t>;
    using bucket_type = channel::one2many_seqnum_bucket<event_t, counter_t>;

public:
    one2many_seqnum_stream_pod_queue(one2many_seqnum_stream_pod_queue&&) noexcept = default;

    one2many_seqnum_stream_pod_queue& operator=(one2many_seqnum_stream_pod_queue&&) noexcept = delete;
    one2many_seqnum_stream_pod_queue(const one2many_seqnum_stream_pod_queue&) = delete;
    one2many_seqnum_stream_pod_queue& operator=(const one2many_seqnum_stream_pod_queue&) = delete;

    bool try_write(event_t&& event, std::memory_order store_order = std::memory_order_release) noexcept
    {
        static_assert(std::is_nothrow_move_constructible<event_t>::value);
        return m_impl.try_write(std::move(event), store_order);
    }

    std::size_t capacity() const noexcept
    {
        return m_impl.capacity();
    }

    std::size_t readers_mask() const noexcept
    {
        return m_impl.readers_mask();
    }

    std::size_t readers_count() const noexcept
    {
        return m_impl.readers_count();
    }

private:
    one2many_seqnum_stream_pod_queue(std::size_t n)
        : m_impl(channel::queue_helper::to2pow(n))
    {
    }

    std::optional<reader_type> create_reader() noexcept
    {
        return m_impl.template create_reader<reader_type>();
    }

private:
    friend class channel_factory;

    channel::one2many_seqnum_stream_queue_impl<event_t, counter_t> m_impl;
};

} // ihft
