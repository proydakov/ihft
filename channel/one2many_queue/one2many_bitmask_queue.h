#pragma once

#include <cmath>
#include <atomic>
#include <memory>
#include <optional>
#include <type_traits>

#include "common.h"

template<typename T>
struct one2many_bitmask_queue_impl
{
    enum : T { MIN_READER_ID = 0 };
    enum : T { MAX_READER_ID = sizeof(T) * 8 - 2 };
    enum : T { MIN_EVENT_SEQ_NUM = 1 };
    enum : T { DUMMY_EVENT_SEQ_NUM = 0 };
    enum : T { EMPTY_DATA_MASK = 0 };
    enum : T { CONSTRUCTED_MASK = T(1) << (MAX_READER_ID + 1) };
};

// predeclaration

// bucket
template<class event_t, typename counter_t>
struct one2many_bitmask_bucket;

// guard
template<class event_t, typename counter_t>
class one2many_bitmask_guard;

// reader
template<class event_t, typename counter_t, typename allocator_t>
class one2many_bitmask_reader;

// queue
template<class event_t, typename counter_t, typename allocator_t>
class one2many_bitmask_queue;

// buffer
template<class event_t, typename counter_t>
using one2many_bitmask_ring_buffer_t = std::shared_ptr<one2many_bitmask_bucket<event_t, counter_t>>;

// implementation

// bucket
template<class event_t, typename counter_t>
struct alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_bitmask_bucket
{
    using storage_t = typename std::aligned_storage<sizeof(event_t), alignof(event_t)>::type;

    one2many_bitmask_bucket() noexcept
        : m_seqn(one2many_bitmask_queue_impl<counter_t>::DUMMY_EVENT_SEQ_NUM)
        , m_mask(one2many_bitmask_queue_impl<counter_t>::EMPTY_DATA_MASK)
        , m_storage{}
    {
    }

    one2many_bitmask_bucket(const one2many_bitmask_bucket&) = delete;
    one2many_bitmask_bucket& operator=(const one2many_bitmask_bucket&) = delete;
    one2many_bitmask_bucket(one2many_bitmask_bucket&&) = delete;
    one2many_bitmask_bucket& operator=(one2many_bitmask_bucket&&) = delete;

    ~one2many_bitmask_bucket() noexcept
    {
        if (m_mask != one2many_bitmask_queue_impl<counter_t>::EMPTY_DATA_MASK)
        {
            get_event().~event_t();
            m_mask.store(one2many_bitmask_queue_impl<counter_t>::EMPTY_DATA_MASK, std::memory_order_relaxed);
        }
    }

    event_t& get_event() noexcept
    {
        return reinterpret_cast<event_t&>(m_storage);
    }

    std::atomic<counter_t> m_seqn;
    std::atomic<counter_t> m_mask;
    storage_t m_storage;
};

template<class event_t, typename counter_t>
using ring_buffer_t = std::shared_ptr<one2many_bitmask_bucket<event_t, counter_t>[]>;

// guard
template<class event_t, typename counter_t>
class one2many_bitmask_guard
{
public:
    one2many_bitmask_guard(one2many_bitmask_bucket<event_t, counter_t>& bucket, counter_t mask) noexcept
        : m_bucket(bucket), m_mask(mask)
    {
    }

    one2many_bitmask_guard(one2many_bitmask_guard&& data)  noexcept
        : m_bucket(data.m_bucket)
        , m_mask(data.m_mask)
    {
        m_mask = one2many_bitmask_queue_impl<counter_t>::EMPTY_DATA_MASK;
    }

    one2many_bitmask_guard& operator=(one2many_bitmask_guard&& data) = delete;
    one2many_bitmask_guard(const one2many_bitmask_guard&) = delete;
    one2many_bitmask_guard& operator=(const one2many_bitmask_guard&) = delete;

    ~one2many_bitmask_guard() noexcept
    {
        if (m_mask != one2many_bitmask_queue_impl<counter_t>::EMPTY_DATA_MASK)
        {
            auto const release_etalon(m_mask | one2many_bitmask_queue_impl<counter_t>::CONSTRUCTED_MASK);
            auto const before = m_bucket.m_mask.fetch_and(~m_mask, std::memory_order_relaxed);

            if (before == release_etalon)
            {
                m_bucket.get_event().~event_t();
                m_bucket.m_mask.store(one2many_bitmask_queue_impl<counter_t>::EMPTY_DATA_MASK, std::memory_order_release);
            }
        }
    }

    event_t const& get_event() const
    {
        return m_bucket.get_event();
    }

private:
    one2many_bitmask_bucket<event_t, counter_t>& m_bucket;
    counter_t m_mask;
};

// reader
template<class event_t, typename counter_t, typename allocator_t>
class alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_bitmask_reader
{
public:
    using guard_type = one2many_bitmask_guard<event_t, counter_t>;
    using ring_buffer_t = one2many_bitmask_ring_buffer_t<event_t, counter_t>;
    using allocator_ptr = std::shared_ptr<allocator_t>;

public:
    one2many_bitmask_reader(allocator_ptr allocator, ring_buffer_t storage, std::size_t storage_mask, counter_t read_from, counter_t mask)
        : m_allocator(std::move(allocator))
        , m_storage(std::move(storage))
        , m_next_bucket(read_from & storage_mask)
        , m_storage_mask(storage_mask)
        , m_next_read_index(read_from)
        , m_reader_mask(mask)
    {
        static_assert(sizeof(one2many_bitmask_reader<event_t, counter_t, allocator_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
    }

    one2many_bitmask_reader(const one2many_bitmask_reader&) = delete;
    one2many_bitmask_reader& operator=(const one2many_bitmask_reader&) = delete;
    one2many_bitmask_reader(one2many_bitmask_reader&&) noexcept = default;
    one2many_bitmask_reader& operator=(one2many_bitmask_reader&&) noexcept = default;

    ~one2many_bitmask_reader() noexcept = default;

    std::optional<guard_type> try_read() noexcept
    {
        auto& bucket = m_storage.get()[m_next_bucket];
        if (bucket.m_seqn.load(std::memory_order_acquire) == m_next_read_index)
        {
            m_next_read_index++;
            m_next_bucket = m_next_read_index & m_storage_mask;
            return std::optional<guard_type>(guard_type{bucket, m_reader_mask});
        }
        else
        {
            return std::nullopt;
        }
    }

    one2many_bitmask_guard<event_t, counter_t> read() noexcept
    {
        read_mark:

        auto opt = try_read();
        if (opt)
        {
            return std::move(*opt);
        }

        goto read_mark;
    }

    counter_t get_id() const noexcept
    {
        return static_cast<counter_t>(std::log2(m_reader_mask));
    }

private:
    allocator_ptr m_allocator;
    ring_buffer_t m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_read_index;
    counter_t m_reader_mask;
};

// queue
template<class event_t, typename counter_t, typename allocator_t = empty_allocator>
class alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_bitmask_queue
{
public:
    using writer_type = one2many_bitmask_queue<event_t, counter_t, allocator_t>;
    using reader_type = one2many_bitmask_reader<event_t, counter_t, allocator_t>;
    using ring_buffer_t = one2many_bitmask_ring_buffer_t<event_t, counter_t>;
    using allocator_ptr = std::shared_ptr<allocator_t>;
    using bucket_type = one2many_bitmask_bucket<event_t, counter_t>;
    using guard_type = one2many_bitmask_guard<event_t, counter_t>;
    using event_type = event_t;

public:
    one2many_bitmask_queue(std::size_t n, allocator_t&& allocator = allocator_t())
        : m_next_bucket(one2many_bitmask_queue_impl<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_storage_mask(calc_mask(n))
        , m_next_seq_num(one2many_bitmask_queue_impl<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_next_reader_id(one2many_bitmask_queue_impl<counter_t>::MIN_READER_ID)
    {
        static_assert(sizeof(one2many_bitmask_queue<event_t, counter_t, allocator_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);

        m_allocator = std::make_shared<allocator_t>(std::move(allocator));

        m_storage.reset(new bucket_type[n], [](bucket_type* ptr){
            delete [] ptr;
        });
    }

    one2many_bitmask_queue(const one2many_bitmask_queue&) = delete;
    one2many_bitmask_queue& operator=(const one2many_bitmask_queue&) = delete;

    one2many_bitmask_queue(one2many_bitmask_queue&&) noexcept = default;
    one2many_bitmask_queue& operator=(one2many_bitmask_queue&&) noexcept = default;

    reader_type create_reader()
    {
        /// @todo : think about the ability to dynamically create readers

        auto const next_id = m_next_reader_id++;
        if (next_id > one2many_bitmask_queue_impl<counter_t>::MAX_READER_ID)
        {
            throw std::runtime_error("Next reader id overflow: " + std::to_string(next_id) + " > " + std::to_string(one2many_bitmask_queue_impl<counter_t>::MAX_READER_ID));
        }

        counter_t const mask(counter_t(1) << next_id);
        return {m_allocator, m_storage, m_storage_mask, m_next_seq_num, mask};
    }

    bool try_write(event_t&& event, std::memory_order store_order = std::memory_order_release) noexcept
    {
        static_assert(std::is_nothrow_move_constructible<event_t>::value);

        auto& bucket = m_storage.get()[m_next_bucket];
        if (bucket.m_mask.load(std::memory_order_acquire) == one2many_bitmask_queue_impl<counter_t>::EMPTY_DATA_MASK)
        {
            auto const alive_mask = (~((~counter_t(0)) << m_next_reader_id)) | one2many_bitmask_queue_impl<counter_t>::CONSTRUCTED_MASK;
            auto const seqn = m_next_seq_num++;
            m_next_bucket = m_next_seq_num & m_storage_mask;
            new (&bucket.m_storage) event_t(std::move(event));
            bucket.m_mask.store(alive_mask, std::memory_order_relaxed);
            bucket.m_seqn.store(seqn, store_order);

            return true;
        }
        else
        {
            return false;
        }
    }

    void write(event_t&& obj) noexcept
    {
        while (!try_write(std::move(obj)));
    }

    std::size_t size() const noexcept
    {
        return m_storage_mask + 1;
    }

    counter_t get_alive_mask() const noexcept
    {
        return (~((~counter_t(0)) << m_next_reader_id));
    }

    allocator_t& get_allocator() noexcept
    {
        return *m_allocator;
    }

private:
    static std::size_t calc_mask(std::size_t n)
    {
        if (n > 0 && 0 == ((n - 1) & n))
        {
            return n - 1;
        }
        throw std::invalid_argument("queue size should be pow of 2.");
    }

private:
    allocator_ptr m_allocator;
    ring_buffer_t m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_seq_num;
    counter_t m_next_reader_id;
};
