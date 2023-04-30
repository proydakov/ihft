#pragma once

#include "private/allocator_holder.h"
#include "private/one2each_seqnum_stream_queue_impl.h"
#include "channel_concept.h"

#include <atomic>
#include <memory>
#include <optional>
#include <type_traits>

namespace ihft::channel
{

// predeclaration

class channel_factory;

// guard
template<complex_event event_t, seqnum_counter counter_t>
class one2each_seqnum_stream_object_guard;

// reader
template<complex_event event_t, seqnum_counter counter_t>
class one2each_seqnum_stream_object_reader;

// queue
template<complex_event event_t, typename content_allocator_t, seqnum_counter counter_t>
class one2each_seqnum_stream_object_queue;

// implementation

// guard
template<complex_event event_t, seqnum_counter counter_t>
class one2each_seqnum_stream_object_guard final
{
public:
    using bucket_type = impl::one2each_seqnum_bucket<event_t, counter_t>;
    using counter_type = impl::one2each_seqnum_queue_constant<counter_t>;

public:
    one2each_seqnum_stream_object_guard(bucket_type& bucket, counter_t owner) noexcept
        : m_bucket(bucket)
        , m_owner(owner)
    {
    }

    one2each_seqnum_stream_object_guard(one2each_seqnum_stream_object_guard&& data) noexcept
        : m_bucket(data.m_bucket)
        , m_owner(data.m_owner)
    {
        data.m_owner = counter_type::DUMMY_READER_ID;
    }

    one2each_seqnum_stream_object_guard& operator=(one2each_seqnum_stream_object_guard&& data) = delete;
    one2each_seqnum_stream_object_guard(const one2each_seqnum_stream_object_guard&) = delete;
    one2each_seqnum_stream_object_guard& operator=(const one2each_seqnum_stream_object_guard&) = delete;

    ~one2each_seqnum_stream_object_guard() noexcept
    {
        if (m_owner != counter_type::DUMMY_READER_ID)
        {
            auto constexpr release_etalon(counter_type::CONSTRUCTED_DATA_MARK + 1);
            auto const before = m_bucket.m_counter.fetch_sub(1, std::memory_order_relaxed);

            if (before == release_etalon)
            {
                m_bucket.get_event().~event_t();
                m_bucket.m_counter.store(counter_type::EMPTY_DATA_MARK, std::memory_order_relaxed);
            }
        }
    }

    operator event_t const& () const noexcept
    {
        return m_bucket.get_event();
    }

private:
    bucket_type& m_bucket;
    counter_t m_owner;
};

template<complex_event event_t, seqnum_counter counter_t>
class alignas(constant::CPU_CACHE_LINE_SIZE) one2each_seqnum_stream_object_reader final
{
public:
    using guard_type = one2each_seqnum_stream_object_guard<event_t, counter_t>;
    using ring_buffer_type = impl::one2each_seqnum_stream_ring_buffer_t<event_t, counter_t>;
    using counter_type = impl::one2each_seqnum_queue_constant<counter_t>;

public:
    one2each_seqnum_stream_object_reader(one2each_seqnum_stream_object_reader&&) noexcept = default;

    one2each_seqnum_stream_object_reader& operator=(one2each_seqnum_stream_object_reader&&) noexcept = delete;
    one2each_seqnum_stream_object_reader(const one2each_seqnum_stream_object_reader&) = delete;
    one2each_seqnum_stream_object_reader& operator=(const one2each_seqnum_stream_object_reader&) = delete;

    std::optional<guard_type> try_read() noexcept
    {
        auto& bucket = m_storage.get()[m_next_bucket];
        counter_t const next = m_next_seq_num & counter_type::SEQNUM_MASK;
        if (bucket.m_seqn.load(std::memory_order_acquire) == next)
        {
            m_next_seq_num++;
            m_next_bucket = m_next_seq_num & m_storage_mask;
            return std::optional<guard_type>(guard_type(bucket, m_id));
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
    one2each_seqnum_stream_object_reader(ring_buffer_type storage, std::size_t storage_mask, counter_t id) noexcept
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

    ring_buffer_type m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_seq_num;
    counter_t m_id;
};

template<complex_event event_t, typename content_allocator_t = impl::empty_allocator, seqnum_counter counter_t = std::uint64_t>
class alignas(constant::CPU_CACHE_LINE_SIZE) one2each_seqnum_stream_object_queue final : public impl::allocator_holder<content_allocator_t>
{
public:
    using allocator_type = content_allocator_t;
    using reader_type = one2each_seqnum_stream_object_reader<event_t, counter_t>;
    using counter_type = impl::one2each_seqnum_queue_constant<counter_t>;

public:
    one2each_seqnum_stream_object_queue(one2each_seqnum_stream_object_queue&&) noexcept = default;

    one2each_seqnum_stream_object_queue& operator=(one2each_seqnum_stream_object_queue&&) noexcept = delete;
    one2each_seqnum_stream_object_queue(const one2each_seqnum_stream_object_queue&) = delete;
    one2each_seqnum_stream_object_queue& operator=(const one2each_seqnum_stream_object_queue&) = delete;

    bool try_write(event_t&& event) noexcept
    {
        static_assert(std::is_nothrow_move_constructible_v<event_t>);
        counter_t const counter = static_cast<counter_t>(m_impl.readers_count()) + counter_type::CONSTRUCTED_DATA_MARK;
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
    // empty_allocator ctor
    template<typename A = content_allocator_t> requires (std::is_same_v<A, impl::empty_allocator>)
    one2each_seqnum_stream_object_queue(std::size_t n)
        : m_impl(impl::channel_helper::to2pow<counter_t>(n))
    {
        static_assert(sizeof(decltype(*this)) <= constant::CPU_CACHE_LINE_SIZE);
    }

    // custom content allocator ctor
    template<typename deleter_t = std::default_delete<content_allocator_t>, typename A = content_allocator_t> requires (!std::is_same_v<A, impl::empty_allocator>)
    one2each_seqnum_stream_object_queue(std::size_t n, std::unique_ptr<content_allocator_t, deleter_t> content_allocator)
        : impl::allocator_holder<content_allocator_t>(content_allocator.get())
        , m_impl(impl::channel_helper::to2pow<counter_t>(n), std::move(content_allocator))
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
