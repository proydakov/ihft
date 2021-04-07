#pragma once

#include <atomic>
#include <type_traits>

#include "common.h"

namespace ihft
{

// bucket
template<class event_t, typename counter_t>
struct alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_seqnum_bucket final
{
    using storage_t = typename std::aligned_storage<sizeof(event_t), alignof(event_t)>::type;

    one2many_seqnum_bucket() noexcept
        : m_seqn(impl::one2many_counter_queue_constant<counter_t>::DUMMY_EVENT_SEQ_NUM)
        , m_counter(impl::one2many_counter_queue_constant<counter_t>::EMPTY_DATA_MARK)
    {
    }

    one2many_seqnum_bucket(const one2many_seqnum_bucket&) = delete;
    one2many_seqnum_bucket& operator=(const one2many_seqnum_bucket&) = delete;
    one2many_seqnum_bucket(one2many_seqnum_bucket&&) = delete;
    one2many_seqnum_bucket& operator=(one2many_seqnum_bucket&&) = delete;

    ~one2many_seqnum_bucket() noexcept
    {
        if (m_counter != impl::one2many_counter_queue_constant<counter_t>::EMPTY_DATA_MARK)
        {
            get_event().~event_t();
            m_counter.store(impl::one2many_counter_queue_constant<counter_t>::EMPTY_DATA_MARK, std::memory_order_relaxed);
        }
    }

    event_t& get_event() noexcept
    {
        return reinterpret_cast<event_t&>(m_storage);
    }

    std::atomic<counter_t> m_seqn;
    std::atomic<counter_t> m_counter;
    storage_t m_storage;
};

}
