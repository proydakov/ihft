#pragma once

#include <cstddef>

namespace ihft::types
{

///
/// A simple pointer life-time control type.
/// It has functionality similar to unique_ptr, but it doesn't call a delete,
/// instead of this it calls destroy_at at destructor.
/// It is useful with manually memory placed objects.
///

template<typename T>
class box final
{
public:
    explicit box(T* iptr)
        : m_ptr(iptr)
    {
    }

    explicit box(std::nullptr_t) = delete;

    ~box() noexcept
    {
        if (has_value())
        {
            std::destroy_at(m_ptr);
        }
    }

    box(box&& other) noexcept
        : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    box& operator=(box&& data) noexcept = delete;

    box(const box&) = delete;
    box& operator=(const box&) = delete;

    bool has_value() const noexcept
    {
        return m_ptr != nullptr;
    }

    operator T const& () const noexcept
    {
        return *m_ptr;
    }

private:
    T* m_ptr;
};

}
