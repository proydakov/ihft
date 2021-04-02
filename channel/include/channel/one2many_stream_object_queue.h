#pragma once

#include <atomic>
#include <memory>
#include <optional>
#include <type_traits>

#include "one2many_counter_bucket.h"

namespace ihft
{

// predeclaration

// guard
template<class event_t, typename counter_t>
class one2many_stream_object_guard;

// buffer
template<class event_t, typename counter_t>
using one2many_stream_object_ring_buffer_t = std::shared_ptr<one2many_counter_bucket<event_t, counter_t>>;

// reader
template<class event_t, typename content_allocator_t, typename counter_t>
class one2many_stream_object_reader;

// queue
template<class event_t, typename content_allocator_t, typename counter_t>
class one2many_stream_object_queue;

// implementation

// guard
template<class event_t, typename counter_t>
class one2many_stream_object_guard final
{
public:
    using bucket_type = one2many_counter_bucket<event_t, counter_t>;

    one2many_stream_object_guard(bucket_type& bucket, counter_t owner) noexcept
        : m_bucket(bucket)
        , m_owner(owner)
    {
    }

    one2many_stream_object_guard(one2many_stream_object_guard&& data) noexcept
        : m_bucket(data.m_bucket)
        , m_owner(data.m_owner)
    {
        data.m_owner = one2many_counter_queue_impl<counter_t>::DUMMY_READER_ID;
    }

    one2many_stream_object_guard& operator=(one2many_stream_object_guard&& data) = delete;
    one2many_stream_object_guard(const one2many_stream_object_guard&) = delete;
    one2many_stream_object_guard& operator=(const one2many_stream_object_guard&) = delete;

    ~one2many_stream_object_guard() noexcept
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
    bucket_type& m_bucket;
    counter_t m_owner;
};

namespace impl
{

template<class event_t, typename counter_t>
struct one2many_stream_object_reader_impl final
{
    using guard_type = one2many_stream_object_guard<event_t, counter_t>;
    using ring_buffer_t = one2many_stream_object_ring_buffer_t<event_t, counter_t>;

    one2many_stream_object_reader_impl(ring_buffer_t storage, std::size_t storage_mask, counter_t read_from, counter_t id) noexcept
        : m_storage(std::move(storage))
        , m_next_bucket(read_from & storage_mask)
        , m_storage_mask(storage_mask)
        , m_next_read_index(read_from)
        , m_id(id)
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

    ring_buffer_t m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_read_index;
    counter_t m_id;
};

template<class event_t, typename counter_t>
struct one2many_stream_object_queue_impl final
{
    using ring_buffer_t = one2many_stream_object_ring_buffer_t<event_t, counter_t>;
    using bucket_type = one2many_counter_bucket<event_t, counter_t>;

    one2many_stream_object_queue_impl(std::size_t n)
        : m_next_bucket(one2many_counter_queue_impl<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_storage_mask(0)
        , m_next_seq_num(one2many_counter_queue_impl<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_next_reader_id(one2many_counter_queue_impl<counter_t>::MIN_READER_ID)
    {
        n = queue_helper::to2pow(n);
        m_storage.reset(new bucket_type[n], [](bucket_type* ptr){
            delete [] ptr;
        });
        m_storage_mask = n - 1;
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

    std::size_t size() const noexcept
    {
        return m_storage_mask + 1;
    }

    counter_t get_readers_mask() const noexcept
    {
        return (~((~counter_t(0)) << m_next_reader_id));
    }

    ring_buffer_t m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_seq_num;
    counter_t m_next_reader_id;
};

template<typename content_allocator_t>
struct one2many_stream_object_allocator_holder
{
public:
    using content_allocator_ptr = std::shared_ptr<content_allocator_t>;

    one2many_stream_object_allocator_holder(content_allocator_ptr ptr)
        : m_content_allocator(std::move(ptr))
    {
    }

    content_allocator_t& get_content_allocator() noexcept
    {
        return *m_content_allocator;
    }

    content_allocator_ptr m_content_allocator;
};

template<>
struct one2many_stream_object_allocator_holder<empty_allocator>
{
};

}

// reader with content allocator
template<class event_t, typename content_allocator_t, typename counter_t>
class alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_stream_object_reader final : private impl::one2many_stream_object_allocator_holder<content_allocator_t>
{
public:
    using guard_type = one2many_stream_object_guard<event_t, counter_t>;
    using ring_buffer_t = one2many_stream_object_ring_buffer_t<event_t, counter_t>;

private:
    using content_allocator_ptr = std::shared_ptr<content_allocator_t>;

public:
    template <bool IsEnabled = true, typename std::enable_if_t<(IsEnabled && std::is_same_v<content_allocator_t, empty_allocator>), int> = 0>
    one2many_stream_object_reader(ring_buffer_t storage, std::size_t storage_mask, counter_t read_from, counter_t id) noexcept
        : m_impl(std::move(storage), storage_mask, read_from, id)
    {
        static_assert(sizeof(one2many_stream_object_reader<event_t, counter_t, content_allocator_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
    }

    template <bool IsEnabled = true, typename std::enable_if_t<(IsEnabled && !std::is_same_v<content_allocator_t, empty_allocator>), int> = 0>
    one2many_stream_object_reader(content_allocator_ptr content_allocator, ring_buffer_t storage, std::size_t storage_mask, counter_t read_from, counter_t id) noexcept
        : impl::one2many_stream_object_allocator_holder<content_allocator_t>(std::move(content_allocator))
        , m_impl(std::move(storage), storage_mask, read_from, id)
    {
        static_assert(sizeof(one2many_stream_object_reader<event_t, counter_t, content_allocator_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
    }

    one2many_stream_object_reader(one2many_stream_object_reader&&) noexcept = default;

    one2many_stream_object_reader& operator=(one2many_stream_object_reader&&) noexcept = delete;
    one2many_stream_object_reader(const one2many_stream_object_reader&) = delete;
    one2many_stream_object_reader& operator=(const one2many_stream_object_reader&) = delete;

    std::optional<guard_type> try_read() noexcept
    {
        return m_impl.try_read();
    }

    counter_t get_id() const noexcept
    {
        return m_impl.m_id;
    }

private:
    impl::one2many_stream_object_reader_impl<event_t, counter_t> m_impl;
};

// queue with content allocator
template<class event_t, typename content_allocator_t = empty_allocator, typename counter_t = std::uint32_t>
class alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_stream_object_queue final : public impl::one2many_stream_object_allocator_holder<content_allocator_t>
{
public:
    using writer_type = one2many_stream_object_queue<event_t, content_allocator_t, counter_t>;
    using reader_type = one2many_stream_object_reader<event_t, content_allocator_t, counter_t>;
    using ring_buffer_t = one2many_stream_object_ring_buffer_t<event_t, counter_t>;
    using bucket_type = one2many_counter_bucket<event_t, counter_t>;
    using guard_type = one2many_stream_object_guard<event_t, counter_t>;
    using event_type = event_t;

private:
    using base_type = impl::one2many_stream_object_allocator_holder<content_allocator_t>;

public:
    template <bool IsEnabled = true, typename std::enable_if_t<(IsEnabled && std::is_same_v<content_allocator_t, empty_allocator>), int> = 0>
    one2many_stream_object_queue(std::size_t n)
        : m_impl(n)
    {
        static_assert(sizeof(one2many_stream_object_queue<event_t, counter_t, content_allocator_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
    }

    template <bool IsEnabled = true, typename std::enable_if_t<(IsEnabled && !std::is_same_v<content_allocator_t, empty_allocator>), int> = 0>
    one2many_stream_object_queue(std::size_t n, content_allocator_t content_allocator = content_allocator_t())
        : impl::one2many_stream_object_allocator_holder<content_allocator_t>(std::make_shared<content_allocator_t>(std::move(content_allocator)))
        , m_impl(n)
    {
        static_assert(sizeof(one2many_stream_object_queue<event_t, counter_t, content_allocator_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
    }

    one2many_stream_object_queue(one2many_stream_object_queue&&) noexcept = default;

    one2many_stream_object_queue& operator=(one2many_stream_object_queue&&) noexcept = delete;
    one2many_stream_object_queue(const one2many_stream_object_queue&) = delete;
    one2many_stream_object_queue& operator=(const one2many_stream_object_queue&) = delete;

    std::optional<reader_type> create_reader() noexcept
    {
        auto const next_id = m_impl.m_next_reader_id++;
        if (next_id != one2many_counter_queue_impl<counter_t>::DUMMY_READER_ID)
        {
            if constexpr(std::is_same_v<content_allocator_t, empty_allocator>)
            {
                return std::make_optional<reader_type>(m_impl.m_storage, m_impl.m_storage_mask, m_impl.m_next_seq_num, next_id);
            }
            else
            {
                return std::make_optional<reader_type>(base_type::m_content_allocator, m_impl.m_storage, m_impl.m_storage_mask, m_impl.m_next_seq_num, next_id);
            }
        }
        else
        {
            return std::nullopt;
        }
    }

    bool try_write(event_t&& event, std::memory_order store_order = std::memory_order_release) noexcept
    {
        return m_impl.try_write(std::move(event), store_order);
    }

    std::size_t size() const noexcept
    {
        return m_impl.size();
    }

    counter_t get_readers_mask() const noexcept
    {
        return m_impl.get_readers_mask();
    }

private:
    impl::one2many_stream_object_queue_impl<event_t, counter_t> m_impl;
};

} // ihft
