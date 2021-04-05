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

// reader
template<class event_t, typename counter_t>
class one2many_stream_object_reader;

// queue
template<class event_t, typename content_allocator_t, typename counter_t>
class one2many_stream_object_queue;

// buffer
template<class event_t, typename counter_t>
using one2many_stream_object_ring_buffer_t = std::shared_ptr<one2many_counter_bucket<event_t, counter_t>>;

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
        data.m_owner = impl::one2many_counter_queue_constant<counter_t>::DUMMY_READER_ID;
    }

    one2many_stream_object_guard& operator=(one2many_stream_object_guard&& data) = delete;
    one2many_stream_object_guard(const one2many_stream_object_guard&) = delete;
    one2many_stream_object_guard& operator=(const one2many_stream_object_guard&) = delete;

    ~one2many_stream_object_guard() noexcept
    {
        if (m_owner != impl::one2many_counter_queue_constant<counter_t>::DUMMY_READER_ID)
        {
            auto constexpr release_etalon(impl::one2many_counter_queue_constant<counter_t>::CONSTRUCTED_DATA_MARK + 1);
            auto const before = m_bucket.m_counter.fetch_sub(1, std::memory_order_relaxed);

            if (before == release_etalon)
            {
                m_bucket.get_event().~event_t();
                m_bucket.m_counter.store(impl::one2many_counter_queue_constant<counter_t>::EMPTY_DATA_MARK, std::memory_order_release);
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

template<class event_t, typename counter_t>
class alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_stream_object_reader final
{
public:
    using guard_type = one2many_stream_object_guard<event_t, counter_t>;
    using ring_buffer_type = one2many_stream_object_ring_buffer_t<event_t, counter_t>;

public:
    one2many_stream_object_reader(ring_buffer_type storage, std::size_t storage_mask, counter_t read_from, counter_t id) noexcept
        : m_storage(std::move(storage))
        , m_next_bucket(read_from & storage_mask)
        , m_storage_mask(storage_mask)
        , m_next_read_index(read_from)
        , m_id(id)
    {
        static_assert(sizeof(one2many_stream_object_reader<event_t, counter_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
    }

    one2many_stream_object_reader(one2many_stream_object_reader&&) noexcept = default;

    one2many_stream_object_reader& operator=(one2many_stream_object_reader&&) noexcept = delete;
    one2many_stream_object_reader(const one2many_stream_object_reader&) = delete;
    one2many_stream_object_reader& operator=(const one2many_stream_object_reader&) = delete;

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

    counter_t get_id() const noexcept
    {
        return m_id;
    }

private:
    ring_buffer_type m_storage;
    std::size_t m_next_bucket;
    std::size_t m_storage_mask;
    counter_t m_next_read_index;
    counter_t m_id;
};

namespace impl
{

template<class event_t, typename counter_t>
class one2many_stream_object_queue_impl final
{
public:
    using bucket_type = one2many_counter_bucket<event_t, counter_t>;
    using reader_type = one2many_stream_object_reader<event_t, counter_t>;
    using ring_buffer_type = one2many_stream_object_ring_buffer_t<event_t, counter_t>;

    template<typename T, typename D>
    one2many_stream_object_queue_impl(std::size_t n, T content_allocator, D content_allocator_deleter)
        : m_storage(new bucket_type[n], [allocator = std::move(content_allocator), deleter = std::move(content_allocator_deleter)](bucket_type* ptr) {
            delete [] ptr;
            // data removed. now we ready to cleanup allocator memory
            deleter(allocator);
        })
        , m_next_bucket(impl::one2many_counter_queue_constant<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_storage_mask(0)
        , m_next_seq_num(impl::one2many_counter_queue_constant<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_next_reader_id(impl::one2many_counter_queue_constant<counter_t>::MIN_READER_ID)
    {
        m_storage_mask = n - 1;
    }

    one2many_stream_object_queue_impl(std::size_t n)
        : m_storage(new bucket_type[n], [](bucket_type* ptr){
            delete [] ptr;
        })
        , m_next_bucket(impl::one2many_counter_queue_constant<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_storage_mask(0)
        , m_next_seq_num(impl::one2many_counter_queue_constant<counter_t>::MIN_EVENT_SEQ_NUM)
        , m_next_reader_id(impl::one2many_counter_queue_constant<counter_t>::MIN_READER_ID)
    {
        m_storage_mask = n - 1;
    }

    std::optional<reader_type> create_reader() noexcept
    {
        auto const next_id = m_next_reader_id++;
        if (m_next_seq_num == impl::one2many_counter_queue_constant<counter_t>::MIN_EVENT_SEQ_NUM and next_id != impl::one2many_counter_queue_constant<counter_t>::DUMMY_READER_ID)
        {
            return std::make_optional<reader_type>(m_storage, m_storage_mask, m_next_seq_num, next_id);
        }
        else
        {
            m_next_reader_id--;
            return std::nullopt;
        }
    }

    bool try_write(event_t&& event, std::memory_order store_order = std::memory_order_release) noexcept
    {
        static_assert(std::is_nothrow_move_constructible<event_t>::value);

        auto& bucket = m_storage.get()[m_next_bucket];
        if (bucket.m_counter.load(std::memory_order_acquire) == impl::one2many_counter_queue_constant<counter_t>::EMPTY_DATA_MARK)
        {
            auto const counter = m_next_reader_id + impl::one2many_counter_queue_constant<counter_t>::CONSTRUCTED_DATA_MARK;
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

template<class event_t, typename content_allocator_t = impl::empty_allocator, typename counter_t = std::uint32_t>
class alignas(QUEUE_CPU_CACHE_LINE_SIZE) one2many_stream_object_queue final : public impl::stream_object_allocator_holder<content_allocator_t>
{
public:
    using reader_type = one2many_stream_object_reader<event_t, counter_t>;
    using allocator_type = content_allocator_t;

public:
    // empty_allocator ctor
    template<bool IsEnabled = true, typename std::enable_if_t<(IsEnabled && std::is_same_v<content_allocator_t, impl::empty_allocator>), int> = 0>
    one2many_stream_object_queue(std::size_t n)
        : m_impl(impl::queue_helper::to2pow(n))
    {
        static_assert(sizeof(one2many_stream_object_queue<event_t, counter_t, content_allocator_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
    }

    // custom content allocator ctor
    template<typename Deleter = std::default_delete<content_allocator_t>, bool IsEnabled = true, typename std::enable_if_t<(IsEnabled && !std::is_same_v<content_allocator_t, impl::empty_allocator>), int> = 0>
    one2many_stream_object_queue(std::size_t n, content_allocator_t* content_allocator, Deleter deleter)
        : impl::stream_object_allocator_holder<content_allocator_t>(content_allocator)
        , m_impl(impl::queue_helper::to2pow(n), content_allocator, std::move(deleter))
    {
        static_assert(sizeof(one2many_stream_object_queue<event_t, counter_t, content_allocator_t>) <= QUEUE_CPU_CACHE_LINE_SIZE);
    }

    one2many_stream_object_queue(one2many_stream_object_queue&&) noexcept = default;

    one2many_stream_object_queue& operator=(one2many_stream_object_queue&&) noexcept = delete;
    one2many_stream_object_queue(const one2many_stream_object_queue&) = delete;
    one2many_stream_object_queue& operator=(const one2many_stream_object_queue&) = delete;

    std::optional<reader_type> create_reader() noexcept
    {
        return m_impl.create_reader();
    }

    bool try_write(event_t&& event, std::memory_order store_order = std::memory_order_release) noexcept
    {
        return m_impl.try_write(std::move(event), store_order);
    }

    std::size_t capacity() const noexcept
    {
        return m_impl.capacity();
    }

    std::size_t readers_mask() const noexcept
    {
        return m_impl.readers_mask();
    }

    std::size_t readers_count() const noexcept
    {
        return m_impl.readers_count();
    }

private:
    impl::one2many_stream_object_queue_impl<event_t, counter_t> m_impl;
};

} // ihft
