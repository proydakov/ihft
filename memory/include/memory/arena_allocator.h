#pragma once

#include <cstddef>
#include <cstdint>

namespace ihft::memory
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
            // All arithmetic is done on integer addresses: forming pointers
            // outside the arena region would be undefined behavior.
            auto const begin = reinterpret_cast<std::uintptr_t>(m_arena.m_origin_arena_ptr);
            auto const end = begin + m_arena.m_arena_size;
            auto const current = reinterpret_cast<std::uintptr_t>(m_arena.m_current_arena_ptr);

            auto const misalign = current % alignof(T);
            auto const aligned = (misalign == 0) ? current : current + (alignof(T) - misalign);

            // The alignment padding doesn't fit or the request is too big.
            // Do not change the arena state in that case. The division guards
            // against overflow of n * sizeof(T).
            if (aligned > end || n > (end - aligned) / sizeof(T))
            {
                return nullptr;
            }

            m_arena.m_current_arena_ptr = reinterpret_cast<char*>(aligned + n * sizeof(T));
            return reinterpret_cast<T*>(aligned);
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

    template<size_t N>
    explicit arena_allocator(char (&charArr)[N])
        : arena_allocator(charArr, sizeof(charArr))
    {
    }

    // The typed_arena_allocator contains a reference into main arena_allocator
    arena_allocator(arena_allocator const&) = delete;
    arena_allocator& operator=(arena_allocator const&) = delete;

    arena_allocator(arena_allocator&&) noexcept = delete;
    arena_allocator& operator=(arena_allocator&&) noexcept = delete;

    template<typename T>
    typed_arena_allocator<T> typed_allocator() noexcept
    {
        return typed_arena_allocator<T>(*this);
    }

    // Reset the current pointer and reuse memory
    // This method doesn't call any dtors
    // The user must call dtors itself before

    void reset() noexcept
    {
        m_current_arena_ptr = static_cast<char*>(m_origin_arena_ptr);
    }

    size_t used() const noexcept
    {
        return static_cast<size_t>(m_current_arena_ptr - m_origin_arena_ptr);
    }

private:
    char* const m_origin_arena_ptr;
    char* m_current_arena_ptr;
    const size_t m_arena_size;
};

}
