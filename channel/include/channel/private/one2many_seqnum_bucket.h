#pragma once

#include <atomic>
#include <type_traits>

#include "common.h"

namespace ihft::channel
{

template<typename T>
struct one2many_seqnum_queue_constant
{
    enum : T { MIN_EVENT_SEQ_NUM = 1 };
    enum : T { DUMMY_EVENT_SEQ_NUM = 0 };
    enum : T { MIN_READER_ID = 0 };
    enum : T { DUMMY_READER_ID = 4096 };
    enum : T { EMPTY_DATA_MARK = 0 };
    enum : T { CONSTRUCTED_DATA_MARK = 1 };
};

// bucket
template<class event_t, typename counter_t>
struct alignas(constant::CPU_CACHE_LINE_SIZE) one2many_seqnum_bucket final
{
    using storage_t = typename std::aligned_storage<sizeof(event_t), alignof(event_t)>::type;

    one2many_seqnum_bucket() noexcept
        : m_seqn(one2many_seqnum_queue_constant<counter_t>::DUMMY_EVENT_SEQ_NUM)
        , m_counter(one2many_seqnum_queue_constant<counter_t>::EMPTY_DATA_MARK)
    {
    }

    one2many_seqnum_bucket(const one2many_seqnum_bucket&) = delete;
    one2many_seqnum_bucket& operator=(const one2many_seqnum_bucket&) = delete;
    one2many_seqnum_bucket(one2many_seqnum_bucket&&) = delete;
    one2many_seqnum_bucket& operator=(one2many_seqnum_bucket&&) = delete;

    ~one2many_seqnum_bucket() noexcept
    {
        if (m_counter != one2many_seqnum_queue_constant<counter_t>::EMPTY_DATA_MARK)
        {
            get_event().~event_t();
            m_counter.store(one2many_seqnum_queue_constant<counter_t>::EMPTY_DATA_MARK, std::memory_order_release);
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
