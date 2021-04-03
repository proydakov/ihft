#pragma once

#include <cstdint>

namespace ihft
{

constexpr std::size_t QUEUE_CPU_CACHE_LINE_SIZE = 64;

template<typename T>
struct one2one_counter_queue_impl
{
    enum : T { MIN_EVENT_SEQ_NUM = 1 };
    enum : T { DUMMY_EVENT_SEQ_NUM = 0 };
    enum : T { MIN_READER_ID = 0 };
    enum : T { DUMMY_READER_ID = 1 };
    enum : T { EMPTY_DATA_MARK = 0 };
};

template<typename T>
struct one2many_counter_queue_impl
{
    enum : T { MIN_EVENT_SEQ_NUM = 1 };
    enum : T { DUMMY_EVENT_SEQ_NUM = 0 };
    enum : T { MIN_READER_ID = 0 };
    enum : T { DUMMY_READER_ID = 4096 };
    enum : T { EMPTY_DATA_MARK = 0 };
    enum : T { CONSTRUCTED_DATA_MARK = 1 };
};

namespace impl
{

struct queue_helper
{
    constexpr static std::size_t to2pow(std::size_t n) noexcept
    {
        if (0 == n)
        {
            return 1;
        }

        constexpr auto max_size = std::size_t(2) << 31;
        if (n >= max_size)
        {
            return max_size;
        }

        std::size_t power = 1;
        while(power < n)
        {
            power *= 2;
        }
        return power;
    }
};

struct empty_allocator
{
};

template<typename content_allocator_t>
struct stream_object_allocator_holder
{
public:
    stream_object_allocator_holder(content_allocator_t* ptr)
        : m_content_allocator(ptr)
    {
    }

    ~stream_object_allocator_holder()
    {
        m_content_allocator = nullptr;
    }

    content_allocator_t& get_content_allocator() noexcept
    {
        return *m_content_allocator;
    }

    content_allocator_t* m_content_allocator;
};

template<>
struct stream_object_allocator_holder<empty_allocator>
{
};

}

} // ihft
