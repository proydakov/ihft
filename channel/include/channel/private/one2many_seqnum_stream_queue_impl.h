#pragma once

#include "ring_buffer_factory.h"
#include "one2many_seqnum_bucket.h"

namespace ihft::channel
{

// buffer
template<typename event_t, typename counter_t>
using one2many_seqnum_stream_ring_buffer_t = std::shared_ptr<one2many_seqnum_bucket<event_t, counter_t>>;

template<typename event_t, typename counter_t>
class one2many_seqnum_stream_queue_impl final
{
public:
    using bucket_type = one2many_seqnum_bucket<event_t, counter_t>;
    using ring_buffer_type = one2many_seqnum_stream_ring_buffer_t<event_t, counter_t>;

    // CA - content allocator type
    // CD - content deleter type
    // RA - region allocator type. Used for ring buffer allocation
    template<typename CA, typename CD, typename RA = std::allocator<bucket_type>>
    one2many_seqnum_stream_queue_impl(std::size_t n, std::unique_ptr<CA, CD> content_allocator, RA region_allocator = RA())
        : m_storage(ring_buffer_factory::make(n, std::move(content_allocator), std::move(region_allocator)))
        , m_next_bucket(one2many_seqnum_queue_constant<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_storage_mask(n - 1)
        , m_next_seq_num(one2many_seqnum_queue_constant<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_next_reader_id(one2many_seqnum_queue_constant<counter_t>::MIN_READER_ID)
    {
    }

    // RA - region allocator type. Used for ring buffer allocation
    template<typename RA = std::allocator<bucket_type>>
    one2many_seqnum_stream_queue_impl(std::size_t n, RA region_allocator = RA())
        : m_storage(ring_buffer_factory::make(n, std::move(region_allocator)))
        , m_next_bucket(one2many_seqnum_queue_constant<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_storage_mask(n - 1)
        , m_next_seq_num(one2many_seqnum_queue_constant<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_next_reader_id(one2many_seqnum_queue_constant<counter_t>::MIN_READER_ID)
    {
    }

    template<typename R>
    std::optional<R> create_reader() noexcept
    {
        auto const next_id = m_next_reader_id++;
        if (next_id != one2many_seqnum_queue_constant<counter_t>::DUMMY_READER_ID)
        {
            return R(m_storage, m_storage_mask, m_next_seq_num, next_id);
        }
        else
        {
            m_next_reader_id--;
            return std::nullopt;
        }
    }

    bool try_write(event_t&& event, counter_t counter, std::memory_order store_order = std::memory_order_release) noexcept
    {
        static_assert(std::is_nothrow_move_constructible<event_t>::value);

        auto& bucket = m_storage.get()[m_next_bucket];
        if (bucket.m_counter.load(std::memory_order_acquire) == one2many_seqnum_queue_constant<counter_t>::EMPTY_DATA_MARK)
        {
            auto const seqn = m_next_seq_num++;
            m_next_bucket = m_next_seq_num & m_storage_mask;
            new (&bucket.m_storage) event_t(std::move(event));
            bucket.m_counter.store(counter, std::memory_order_relaxed);
            bucket.m_seqn.store(seqn, store_order);
            return true;
        }
        else
        {
            return false;
        }
    }

    std::size_t capacity() const noexcept
    {
        return m_storage_mask + 1;
    }

    std::size_t readers_mask() const noexcept
    {
        return (~((~std::size_t(0)) << m_next_reader_id));
    }

    std::size_t readers_count() const noexcept
    {
        return m_next_reader_id;
    }

private:
    ring_buffer_type m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_seq_num;
    counter_t m_next_reader_id;
};

}
