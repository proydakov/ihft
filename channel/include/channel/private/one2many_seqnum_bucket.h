#pragma once

#include <atomic>
#include <limits>
#include <type_traits>

#include "channel_helper.h"

namespace ihft::channel::impl
{

template<typename T>
struct one2many_seqnum_queue_constant
{
    static_assert(std::is_unsigned_v<T>, "Counter type must be unsigned.");

    enum : T { MIN_EVENT_SEQ_NUM = 0 };
    enum : T { DUMMY_EVENT_SEQ_NUM = std::numeric_limits<T>::max() };
    enum : T { MIN_READER_ID = 0 };
    enum : T { DUMMY_READER_ID = 255 };
    enum : T { SEQNUM_MASK = std::numeric_limits<T>::max() >> T(1) };
    enum : T { EMPTY_DATA_MARK = 0 };
    enum : T { CONSTRUCTED_DATA_MARK = 1 };
};

static_assert(one2many_seqnum_queue_constant<std::uint8_t>::SEQNUM_MASK == 127ul);
static_assert(one2many_seqnum_queue_constant<std::uint16_t>::SEQNUM_MASK == 32'767ul);
static_assert(one2many_seqnum_queue_constant<std::uint32_t>::SEQNUM_MASK == 2'147'483'647ul);
static_assert(one2many_seqnum_queue_constant<std::uint64_t>::SEQNUM_MASK == 9'223'372'036'854'775'807ul);

// bucket
template<typename event_t, typename counter_t>
struct alignas(constant::CPU_CACHE_LINE_SIZE) one2many_seqnum_bucket final
{
    using storage_type = typename std::aligned_storage<sizeof(event_t), alignof(event_t)>::type;
    using counter_type = one2many_seqnum_queue_constant<counter_t>;

    one2many_seqnum_bucket() noexcept
        : m_seqn(counter_type::DUMMY_EVENT_SEQ_NUM)
        , m_counter(counter_type::EMPTY_DATA_MARK)
    {
    }

    one2many_seqnum_bucket(const one2many_seqnum_bucket&) = delete;
    one2many_seqnum_bucket& operator=(const one2many_seqnum_bucket&) = delete;
    one2many_seqnum_bucket(one2many_seqnum_bucket&&) = delete;
    one2many_seqnum_bucket& operator=(one2many_seqnum_bucket&&) = delete;

    ~one2many_seqnum_bucket() noexcept
    {
        if (m_counter != counter_type::EMPTY_DATA_MARK)
        {
            std::destroy_at(&get_event());
            m_seqn.store(counter_type::DUMMY_EVENT_SEQ_NUM, std::memory_order_relaxed);
            m_counter.store(counter_type::EMPTY_DATA_MARK, std::memory_order_release);
        }
    }

    event_t& get_event() noexcept
    {
        // Note: std::launder is needed after the change of object model in P0137R1
        return *std::launder(reinterpret_cast<event_t*>(&m_storage));
    }

    std::atomic<counter_t> m_seqn;
    std::atomic<counter_t> m_counter;
    storage_type m_storage;
};

}
