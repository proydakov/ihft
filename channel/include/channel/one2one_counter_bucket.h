#pragma once

#include <atomic>
#include <type_traits>

#include "common.h"

namespace ihft
{

// bucket
template<class event_t, typename counter_t>
struct alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2one_counter_bucket final
{
    using storage_t = typename std::aligned_storage<sizeof(event_t), alignof(event_t)>::type;

    one2one_counter_bucket() noexcept
        : m_seqn(one2one_counter_queue_impl<counter_t>::DUMMY_EVENT_SEQ_NUM)
    {
    }

    one2one_counter_bucket(const one2one_counter_bucket&) = delete;
    one2one_counter_bucket& operator=(const one2one_counter_bucket&) = delete;
    one2one_counter_bucket(one2one_counter_bucket&&) = delete;
    one2one_counter_bucket& operator=(one2one_counter_bucket&&) = delete;

    ~one2one_counter_bucket() noexcept
    {
        if (m_seqn != one2one_counter_queue_impl<counter_t>::DUMMY_EVENT_SEQ_NUM)
        {
            get_event().~event_t();
            m_seqn.store(one2one_counter_queue_impl<counter_t>::DUMMY_EVENT_SEQ_NUM, std::memory_order_relaxed);
        }
    }

    event_t& get_event() noexcept
    {
        return reinterpret_cast<event_t&>(m_storage);
    }

    std::atomic<counter_t> m_seqn;
    storage_t m_storage;
};

}
