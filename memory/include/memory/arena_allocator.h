#pragma once

namespace ihft
{

struct arena_allocator
{
    template<typename T>
    struct typed_arena_allocator
    {
        using value_type = T;

        typed_arena_allocator(arena_allocator& arena)
            : m_arena(arena)
        {
        }

        // STL-like interface

        [[nodiscard]] T* allocate(size_t n) noexcept
        {
            if constexpr(alignof(T) > 1)
            {
                static_assert(sizeof(void*) == sizeof(uint64_t));
                uint64_t ost = (uint64_t)(m_arena.m_current_arena_ptr) % alignof(T);
                if (ost > 0) [[unlikely]]
                {
                    m_arena.m_current_arena_ptr += alignof(T) - ost;
                }
            }

            char* test = static_cast<char*>(m_arena.m_current_arena_ptr + n * sizeof(T));
            if (test <= m_arena.m_origin_arena_ptr + m_arena.m_arena_size) [[likely]]
            {
                T* result = reinterpret_cast<T*>(m_arena.m_current_arena_ptr);
                m_arena.m_current_arena_ptr = test;
                return result;
            }
            else [[unlikely]]
            {
                return nullptr;
            }
        }

        void deallocate(T*, size_t) noexcept
        {
        }

    private:
        arena_allocator& m_arena;
    };

    // The arena doesn't own the memory
    // The region could be a huge stack array
    // It also could be an object tail extra data

    arena_allocator(void* arena_start_ptr, size_t arena_size)
        : m_origin_arena_ptr(static_cast<char*>(arena_start_ptr))
        , m_current_arena_ptr(static_cast<char*>(arena_start_ptr))
        , m_arena_size(arena_size)
    {
    }

    // The typed_arena_allocator contains a reference into main arena_allocator
    arena_allocator(arena_allocator const&) = delete;
    arena_allocator& operator=(arena_allocator const&) = delete;

    arena_allocator(arena_allocator&&) noexcept = delete;
    arena_allocator& operator=(arena_allocator&&) noexcept = delete;

    template<typename T>
    typed_arena_allocator<T> typed_allocator()
    {
        return typed_arena_allocator<T>(*this);
    }

    // Reset the current pointer and reuse memory
    // This method doesn't call any dtors
    // The user must call dtors itself before

    void reset()
    {
        m_current_arena_ptr = static_cast<char*>(m_origin_arena_ptr);
    }

private:
    char* const m_origin_arena_ptr;
    char* m_current_arena_ptr;
    const size_t m_arena_size;
};

}
