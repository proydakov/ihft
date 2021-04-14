#pragma once

#include <memory>

#include "private/common.h"

namespace ihft
{
    template<typename T, typename RegionAllocator = std::allocator<T>>
    class stream_fixed_pool_allocator
    {
        struct alignas(memory::CPU_CACHE_LINE_SIZE) holder
        {
            T data;
        };

        using RegionHolderAllocator = typename std::allocator_traits<RegionAllocator>::template rebind_alloc<holder>;

        holder* m_data;
        std::size_t m_next;
        std::size_t const m_size;
        RegionHolderAllocator m_allocator;

    public:
        using value_type = T;

    public:
        stream_fixed_pool_allocator(std::size_t queue_capacity, const RegionAllocator& allocator = RegionAllocator())
            : m_data(nullptr)
            , m_next(0)
            , m_size(queue_capacity + 1)
            , m_allocator(allocator)
        {
            m_data = m_allocator.allocate(m_size);
        }

        ~stream_fixed_pool_allocator()
        {
            m_allocator.deallocate(m_data, m_size);
        }

        T* allocate(std::size_t n)
        {
            if (n == 1)
            {
                auto ptr = active_slab();
                move_to_next_slab();
                return ptr;
            }
            else
            {
                return nullptr;
            }
        }

        void deallocate(T*, std::size_t)
        {
        }

        T* active_slab() const
        {
            auto& res = m_data[m_next];
            return std::addressof(res.data);
        }

        void move_to_next_slab()
        {
            m_next++;
            if (m_next >= m_size)
            {
                m_next = 0;
            }
        }
    };
}
