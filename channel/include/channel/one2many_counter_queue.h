#pragma once

#include <atomic>
#include <memory>
#include <limits>
#include <optional>
#include <type_traits>

#include "common.h"

template<typename T>
struct one2many_counter_queue_impl
{
    enum : T { MIN_EVENT_SEQ_NUM = 1 };
    enum : T { DUMMY_EVENT_SEQ_NUM = 0 };
    enum : T { MIN_READER_ID = 0 };
    enum : T { DUMMY_READER_ID = std::numeric_limits<T>::max() };
    enum : T { EMPTY_DATA_MARK = 0 };
    enum : T { CONSTRUCTED_DATA_MARK = 1 };
};

// predeclaration

// bucket
template<class event_t, typename counter_t>
struct one2many_counter_bucket;

// guard
template<class event_t, typename counter_t>
class one2many_counter_guard;

// reader
template<class event_t, typename counter_t, typename allocator_t>
class one2many_counter_reader;

// queue
template<class event_t, typename counter_t, typename allocator_t>
class one2many_counter_queue;

// buffer
template<class event_t, typename counter_t>
using one2many_counter_ring_buffer_t = std::shared_ptr<one2many_counter_bucket<event_t, counter_t>>;

// implementation

// bucket
template<class event_t, typename counter_t>
struct alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_counter_bucket
{
    using storage_t = typename std::aligned_storage<sizeof(event_t), alignof(event_t)>::type;

    one2many_counter_bucket() noexcept
        : m_seqn(one2many_counter_queue_impl<counter_t>::DUMMY_EVENT_SEQ_NUM)
        , m_counter(one2many_counter_queue_impl<counter_t>::EMPTY_DATA_MARK)
    {
    }

    one2many_counter_bucket(const one2many_counter_bucket&) = delete;
    one2many_counter_bucket& operator=(const one2many_counter_bucket&) = delete;
    one2many_counter_bucket(one2many_counter_bucket&&) = delete;
    one2many_counter_bucket& operator=(one2many_counter_bucket&&) = delete;

    ~one2many_counter_bucket() noexcept
    {
        if (m_counter != one2many_counter_queue_impl<counter_t>::EMPTY_DATA_MARK)
        {
            get_event().~event_t();
            m_counter.store(one2many_counter_queue_impl<counter_t>::EMPTY_DATA_MARK, std::memory_order_relaxed);
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

// guard
template<class event_t, typename counter_t>
class one2many_counter_guard
{
public:
    one2many_counter_guard(one2many_counter_bucket<event_t, counter_t>& bucket, counter_t owner) noexcept
        : m_bucket(bucket)
        , m_owner(owner)
    {
    }

    one2many_counter_guard(one2many_counter_guard&& data) noexcept
        : m_bucket(data.m_bucket)
        , m_owner(data.m_owner)
    {
        data.m_owner = one2many_counter_queue_impl<counter_t>::DUMMY_READER_ID;
    }

    one2many_counter_guard& operator=(one2many_counter_guard&& data) = delete;
    one2many_counter_guard(const one2many_counter_guard&) = delete;
    one2many_counter_guard& operator=(const one2many_counter_guard&) = delete;

    ~one2many_counter_guard() noexcept
    {
        if (m_owner != one2many_counter_queue_impl<counter_t>::DUMMY_READER_ID)
        {
            auto constexpr release_etalon(one2many_counter_queue_impl<counter_t>::CONSTRUCTED_DATA_MARK + 1);
            auto const before = m_bucket.m_counter.fetch_sub(1, std::memory_order_relaxed);

            if (before == release_etalon)
            {
                m_bucket.get_event().~event_t();
                m_bucket.m_counter.store(one2many_counter_queue_impl<counter_t>::EMPTY_DATA_MARK, std::memory_order_release);
            }
        }
    }

    event_t const& get_event() const noexcept
    {
        return m_bucket->get_event();
    }

private:
    one2many_counter_bucket<event_t, counter_t>& m_bucket;
    counter_t m_owner;
};

// reader
template<class event_t, typename counter_t, typename allocator_t>
class alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_counter_reader
{
public:
    using guard_type = one2many_counter_guard<event_t, counter_t>;
    using ring_buffer_t = one2many_counter_ring_buffer_t<event_t, counter_t>;
    using allocator_ptr = std::shared_ptr<allocator_t>;

public:
    // ctor
    one2many_counter_reader(allocator_ptr allocator, ring_buffer_t storage, std::size_t storage_mask, counter_t read_from, counter_t id) noexcept
        : m_allocator(allocator)
        , m_storage(std::move(storage))
        , m_next_bucket(read_from & storage_mask)
        , m_storage_mask(storage_mask)
        , m_next_read_index(read_from)
        , m_id(id)
    {
        static_assert(sizeof(one2many_counter_reader<event_t, counter_t, allocator_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
    }

    one2many_counter_reader(const one2many_counter_reader&) = delete;
    one2many_counter_reader& operator=(const one2many_counter_reader&) = delete;

    one2many_counter_reader(one2many_counter_reader&&) noexcept = default;
    one2many_counter_reader& operator=(one2many_counter_reader&&) noexcept = default;

    ~one2many_counter_reader() noexcept
    {
    }

    std::optional<guard_type> try_read() noexcept
    {
        auto& bucket = m_storage.get()[m_next_bucket];
        if (bucket.m_seqn.load(std::memory_order_acquire) == m_next_read_index)
        {
            m_next_read_index++;
            m_next_bucket = m_next_read_index & m_storage_mask;
            return std::optional<guard_type>(guard_type(bucket, m_id));
        }
        else
        {
            return std::nullopt;
        }
    }

    one2many_counter_guard<event_t, counter_t> read() noexcept
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
        return m_id;
    }

private:
    allocator_ptr m_allocator;
    ring_buffer_t m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_read_index;
    counter_t m_id;
};

// queue
template<class event_t, typename counter_t, typename allocator_t = empty_allocator>
class alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_counter_queue
{
public:
    using writer_type = one2many_counter_queue<event_t, counter_t, allocator_t>;
    using reader_type = one2many_counter_reader<event_t, counter_t, allocator_t>;
    using ring_buffer_t = one2many_counter_ring_buffer_t<event_t, counter_t>;
    using allocator_ptr = std::shared_ptr<allocator_t>;
    using bucket_type = one2many_counter_bucket<event_t, counter_t>;
    using guard_type = one2many_counter_guard<event_t, counter_t>;
    using event_type = event_t;

public:
    one2many_counter_queue(std::size_t n, allocator_t&& allocator = allocator_t())
        : m_next_bucket(one2many_counter_queue_impl<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_storage_mask(calc_mask(n))
        , m_next_seq_num(one2many_counter_queue_impl<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_next_reader_id(one2many_counter_queue_impl<counter_t>::MIN_READER_ID)
    {
        static_assert(sizeof(one2many_counter_queue<event_t, counter_t, allocator_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);

        m_allocator = std::make_shared<allocator_t>(std::move(allocator));

        m_storage.reset(new bucket_type[n], [](bucket_type* ptr){
            delete [] ptr;
        });
    }

    one2many_counter_queue(const one2many_counter_queue&) = delete;
    one2many_counter_queue& operator=(const one2many_counter_queue&) = delete;

    one2many_counter_queue(one2many_counter_queue&&) noexcept = default;
    one2many_counter_queue& operator=(one2many_counter_queue&&) noexcept = default;

    reader_type create_reader()
    {
        auto const next_id = m_next_reader_id++;
        if (next_id == one2many_counter_queue_impl<counter_t>::DUMMY_READER_ID)
        {
            throw std::runtime_error("Next reader id overflow: " + std::to_string(next_id) + " > " + std::to_string(one2many_counter_queue_impl<counter_t>::DUMMY_READER_ID));
        }
        return reader_type(m_allocator, m_storage, m_storage_mask, m_next_seq_num, next_id);
    }

    bool try_write(event_t&& event, std::memory_order store_order = std::memory_order_release) noexcept
    {
        static_assert(std::is_nothrow_move_constructible<event_t>::value);

        auto& bucket = m_storage.get()[m_next_bucket];
        if (bucket.m_counter.load(std::memory_order_acquire) == one2many_counter_queue_impl<counter_t>::EMPTY_DATA_MARK)
        {
            auto const counter = m_next_reader_id + one2many_counter_queue_impl<counter_t>::CONSTRUCTED_DATA_MARK;
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
