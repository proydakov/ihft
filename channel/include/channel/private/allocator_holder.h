#pragma once

namespace ihft::channel::impl
{

struct empty_allocator
{
};

template<typename content_allocator_t>
struct allocator_holder
{
public:
    allocator_holder(content_allocator_t* ptr) noexcept
        : m_content_allocator(ptr)
    {
    }

    ~allocator_holder() noexcept
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
struct allocator_holder<empty_allocator>
{
};

} // ihft::channel::impl
