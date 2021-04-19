#pragma once

#include <atomic>
#include <type_traits>

#include "common.h"

namespace ihft::channel
{

template<typename T>
struct one2one_seqnum_queue_constant
{
    enum : T { MIN_EVENT_SEQ_NUM = 1 };
    enum : T { DUMMY_EVENT_SEQ_NUM = 0 };
    enum : T { MIN_READER_ID = 0 };
    enum : T { DUMMY_READER_ID = 1 };
    enum : T { EMPTY_DATA_MARK = 0 };
};

// bucket
template<class event_t, typename counter_t>
struct alignas(constant::CPU_CACHE_LINE_SIZE) one2one_seqnum_bucket final
{
    using storage_t = typename std::aligned_storage<sizeof(event_t), alignof(event_t)>::type;

    one2one_seqnum_bucket() noexcept
        : m_seqn(one2one_seqnum_queue_constant<counter_t>::DUMMY_EVENT_SEQ_NUM)
    {
    }

    one2one_seqnum_bucket(const one2one_seqnum_bucket&) = delete;
    one2one_seqnum_bucket& operator=(const one2one_seqnum_bucket&) = delete;
    one2one_seqnum_bucket(one2one_seqnum_bucket&&) = delete;
    one2one_seqnum_bucket& operator=(one2one_seqnum_bucket&&) = delete;

    ~one2one_seqnum_bucket() noexcept
    {
        if (m_seqn != one2one_seqnum_queue_constant<counter_t>::DUMMY_EVENT_SEQ_NUM)
        {
            get_event().~event_t();
            m_seqn.store(one2one_seqnum_queue_constant<counter_t>::DUMMY_EVENT_SEQ_NUM, std::memory_order_release);
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
