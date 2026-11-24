#pragma once

#include <constant/constant.h>

#include <memory>
#include <type_traits>
#include <utility>

namespace ihft::memory
{
    template<typename T, typename RegionAllocator = std::allocator<T>>
    class stream_fixed_pool_allocator
    {
        struct alignas(constant::CPU_CACHE_LINE_SIZE) holder
        {
            std::aligned_storage_t<sizeof(T), alignof(T)> data;
        };

        using RegionHolderAllocator = typename std::allocator_traits<RegionAllocator>::template rebind_alloc<holder>;

        holder* m_data;
        std::size_t m_next;
        std::size_t m_size;
        RegionHolderAllocator m_allocator;

    public:
        using value_type = T;

    public:
        stream_fixed_pool_allocator(std::size_t queue_capacity, std::size_t extra, const RegionAllocator& allocator = RegionAllocator())
            : m_data(nullptr)
            , m_next(0)
            // The queue has maximum of queue_capacity elements.
            // Allow the client to prepare the next record before the queue space becomes available
            , m_size(queue_capacity + extra)
            , m_allocator(allocator)
        {
            m_data = m_allocator.allocate(m_size);
        }

        stream_fixed_pool_allocator(std::size_t queue_capacity, const RegionAllocator& allocator = RegionAllocator())
            : stream_fixed_pool_allocator(queue_capacity, 1, allocator)
        {
        }

        ~stream_fixed_pool_allocator() noexcept
        {
            if (m_data != nullptr)
            {
                m_allocator.deallocate(m_data, m_size);
            }
        }

        stream_fixed_pool_allocator(const stream_fixed_pool_allocator&) = delete;

        // Move transfers the ownership of the region, so the moved-from object
        // no longer deallocates it (avoids a double free).
        stream_fixed_pool_allocator(stream_fixed_pool_allocator&& other) noexcept
            : m_data(std::exchange(other.m_data, nullptr))
            , m_next(std::exchange(other.m_next, 0))
            , m_size(std::exchange(other.m_size, 0))
            , m_allocator(std::move(other.m_allocator))
        {
        }

        stream_fixed_pool_allocator& operator=(const stream_fixed_pool_allocator&) = delete;

        stream_fixed_pool_allocator& operator=(stream_fixed_pool_allocator&& other) noexcept
        {
            if (this != &other)
            {
                if (m_data != nullptr)
                {
                    m_allocator.deallocate(m_data, m_size);
                }

                m_data = std::exchange(other.m_data, nullptr);
                m_next = std::exchange(other.m_next, 0);
                m_size = std::exchange(other.m_size, 0);
                m_allocator = std::move(other.m_allocator);
            }
            return *this;
        }

        // STL-like interface

        [[nodiscard]] inline T* allocate(std::size_t n) noexcept
        {
            if (n == 1) [[likely]]
            {
                auto ptr = active_slab();
                seek_to_next_slab();
                return ptr;
            }
            else [[unlikely]]
            {
                return nullptr;
            }
        }

        void deallocate(T*, std::size_t) noexcept
        {
        }

        // IHFT-like interface
        // We are going to next slab only after success producer.try_write()

        [[nodiscard]] T* active_slab() const noexcept
        {
            auto& res = m_data[m_next];
            return reinterpret_cast<T*>(&res.data);
        }

        void seek_to_next_slab() noexcept
        {
            m_next++;
            if (m_next >= m_size) [[unlikely]]
            {
                m_next = 0;
            }
        }

        // Helper section

        std::size_t position() const noexcept
        {
            return m_next;
        }

        std::size_t capacity() const noexcept
        {
            return m_size;
        }
    };
}
