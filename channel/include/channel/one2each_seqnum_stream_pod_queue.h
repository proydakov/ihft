#pragma once

#include "private/one2each_seqnum_stream_queue_impl.h"
#include "channel_concept.h"

#include <atomic>
#include <memory>
#include <optional>
#include <type_traits>

namespace ihft::channel
{

// predeclaration

// reader
template<plain_event event_t, seqnum_counter counter_t>
class one2each_seqnum_stream_pod_reader;

// queue
template<plain_event event_t, seqnum_counter counter_t>
class one2each_seqnum_stream_pod_queue;

// implementation

// reader
template<plain_event event_t, seqnum_counter counter_t = std::uint64_t>
class alignas(constant::CPU_CACHE_LINE_SIZE) one2each_seqnum_stream_pod_reader final
{
public:
    using event_type = event_t;
    using ring_buffer_t = impl::one2each_seqnum_stream_ring_buffer_t<event_t, counter_t>;
    using counter_type = impl::one2each_seqnum_queue_constant<counter_t>;

public:
    one2each_seqnum_stream_pod_reader(one2each_seqnum_stream_pod_reader&&) noexcept = default;

    one2each_seqnum_stream_pod_reader& operator=(one2each_seqnum_stream_pod_reader&&) noexcept = delete;
    one2each_seqnum_stream_pod_reader(const one2each_seqnum_stream_pod_reader&) = delete;
    one2each_seqnum_stream_pod_reader& operator=(const one2each_seqnum_stream_pod_reader&) = delete;

    std::optional<event_t> try_read() noexcept
    {
        auto& bucket = m_storage.get()[m_next_bucket];
        counter_t const next = m_next_seq_num & counter_type::SEQNUM_MASK;
        if (bucket.m_seqn.load(std::memory_order_acquire) == next)
        {
            m_next_seq_num++;
            m_next_bucket = m_next_seq_num & m_storage_mask;
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
    one2each_seqnum_stream_pod_reader(ring_buffer_t storage, std::size_t storage_mask, counter_t id) noexcept
        : m_storage(std::move(storage))
        , m_next_bucket(counter_type::MIN_EVENT_SEQ_NUM & storage_mask)
        , m_storage_mask(storage_mask)
        , m_next_seq_num(counter_type::MIN_EVENT_SEQ_NUM)
        , m_id(id)
    {
        static_assert(sizeof(decltype(*this)) <= constant::CPU_CACHE_LINE_SIZE);
    }

private:
    friend class impl::one2each_seqnum_stream_queue_impl<event_t, counter_t>;

    ring_buffer_t m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_seq_num;
    counter_t m_id;
};

// queue
template<plain_event event_t, seqnum_counter counter_t = std::uint64_t>
class alignas(constant::CPU_CACHE_LINE_SIZE) one2each_seqnum_stream_pod_queue final
{
public:
    using reader_type = one2each_seqnum_stream_pod_reader<event_t, counter_t>;
    using ring_buffer_t = impl::one2each_seqnum_stream_ring_buffer_t<event_t, counter_t>;
    using bucket_type = impl::one2each_seqnum_bucket<event_t, counter_t>;

public:
    one2each_seqnum_stream_pod_queue(one2each_seqnum_stream_pod_queue&&) noexcept = default;

    one2each_seqnum_stream_pod_queue& operator=(one2each_seqnum_stream_pod_queue&&) noexcept = delete;
    one2each_seqnum_stream_pod_queue(const one2each_seqnum_stream_pod_queue&) = delete;
    one2each_seqnum_stream_pod_queue& operator=(const one2each_seqnum_stream_pod_queue&) = delete;

    bool try_write(event_t&& event) noexcept
    {
        counter_t const counter = static_cast<counter_t>(m_impl.readers_count());
        return m_impl.try_write(std::move(event), counter);
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
    one2each_seqnum_stream_pod_queue(std::size_t n)
        : m_impl(impl::channel_helper::to2pow<counter_t>(n))
    {
        static_assert(sizeof(decltype(*this)) <= constant::CPU_CACHE_LINE_SIZE);
    }

    std::optional<reader_type> create_reader() noexcept
    {
        return m_impl.template create_reader<reader_type>();
    }

private:
    friend class channel_factory;

    impl::one2each_seqnum_stream_queue_impl<event_t, counter_t> m_impl;
};

} // ihft
