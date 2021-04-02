#pragma once

#include <atomic>
#include <memory>
#include <optional>
#include <type_traits>

#include "one2many_counter_bucket.h"

namespace ihft
{

// predeclaration

// reader
template<class event_t, typename counter_t>
class one2many_counter_pod_reader;

// queue
template<class event_t, typename counter_t>
class one2many_counter_pod_queue;

// buffer
template<class event_t, typename counter_t>
using one2many_counter_pod_ring_buffer_t = std::shared_ptr<one2many_counter_bucket<event_t, counter_t>>;

// implementation

// reader
template<class event_t, typename counter_t>
class alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_counter_pod_reader final
{
public:
    using ring_buffer_t = one2many_counter_pod_ring_buffer_t<event_t, counter_t>;
    using event_type = event_t;

public:
    // ctor
    one2many_counter_pod_reader(ring_buffer_t storage, std::size_t storage_mask, counter_t read_from, counter_t id) noexcept
        : m_storage(std::move(storage))
        , m_next_bucket(read_from & storage_mask)
        , m_storage_mask(storage_mask)
        , m_next_read_index(read_from)
        , m_id(id)
    {
        static_assert(sizeof(one2many_counter_pod_reader<event_t, counter_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
    }

    one2many_counter_pod_reader(one2many_counter_pod_reader&&) noexcept = default;

    one2many_counter_pod_reader& operator=(one2many_counter_pod_reader&&) noexcept = delete;
    one2many_counter_pod_reader(const one2many_counter_pod_reader&) = delete;
    one2many_counter_pod_reader& operator=(const one2many_counter_pod_reader&) = delete;

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
    ring_buffer_t m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_read_index;
    counter_t m_id;
};

// queue
template<class event_t, typename counter_t>
class alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_counter_pod_queue final
{
public:
    using writer_type = one2many_counter_pod_queue<event_t, counter_t>;
    using reader_type = one2many_counter_pod_reader<event_t, counter_t>;
    using ring_buffer_t = one2many_counter_pod_ring_buffer_t<event_t, counter_t>;
    using bucket_type = one2many_counter_bucket<event_t, counter_t>;
    using event_type = event_t;

public:
    one2many_counter_pod_queue(std::size_t n)
        : m_next_bucket(one2many_counter_queue_impl<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_storage_mask(0)
        , m_next_seq_num(one2many_counter_queue_impl<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_next_reader_id(one2many_counter_queue_impl<counter_t>::MIN_READER_ID)
    {
        static_assert(sizeof(one2many_counter_pod_queue<event_t, counter_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
        static_assert(std::is_trivially_copyable<event_t>::value);

        n = queue_helper::to2pow(n);
        m_storage.reset(new bucket_type[n], [](bucket_type* ptr){
            delete [] ptr;
        });
        m_storage_mask = n - 1;
    }

    one2many_counter_pod_queue(one2many_counter_pod_queue&&) noexcept = default;

    one2many_counter_pod_queue& operator=(one2many_counter_pod_queue&&) noexcept = delete;
    one2many_counter_pod_queue(const one2many_counter_pod_queue&) = delete;
    one2many_counter_pod_queue& operator=(const one2many_counter_pod_queue&) = delete;

    std::optional<reader_type> create_reader() noexcept
    {
        auto const next_id = m_next_reader_id++;
        if (next_id != one2many_counter_queue_impl<counter_t>::DUMMY_READER_ID)
        {
            return std::make_optional<reader_type>(m_storage, m_storage_mask, m_next_seq_num, next_id);
        }
        else
        {
            return std::nullopt;
        }
    }

    bool try_write(event_t&& event, std::memory_order store_order = std::memory_order_release) noexcept
    {
        static_assert(std::is_nothrow_move_constructible<event_t>::value);

        auto& bucket = m_storage.get()[m_next_bucket];
        if (bucket.m_counter.load(std::memory_order_acquire) == one2many_counter_queue_impl<counter_t>::EMPTY_DATA_MARK)
        {
            auto const counter = m_next_reader_id;
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

    std::size_t size() const noexcept
    {
        return m_storage_mask + 1;
    }

    counter_t get_alive_mask() const noexcept
    {
        return (~((~counter_t(0)) << m_next_reader_id));
    }

private:
    ring_buffer_t m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_seq_num;
    counter_t m_next_reader_id;
};

} // ihft