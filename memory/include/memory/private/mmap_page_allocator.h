#pragma once

#include "constant.h"

#include <cstddef>

#include <sys/mman.h>

#ifdef __linux__
#include <linux/mman.h>
#endif

namespace ihft::memory::impl
{

//
// Code below uses MAP_HUGETLB. More details here:
// https://man7.org/linux/man-pages/man2/mmap.2.html
//

template <typename T, unsigned psize, bool is_huge>
struct mmap_page_allocator
{
    using value_type = T;

    static constexpr unsigned page_size = psize;

    mmap_page_allocator()
    {
        // Apple doesn't support hugepages directly, isn't it ?
        // We should check real page alignment for placement

#ifdef __APPLE__
        static_assert(page_size == _4kb_, "Apple supports only the 4kb mmap pages.");
        static_assert(is_huge == false, "Apple doesn't support hugepages.");
#endif

        static_assert(psize % alignof(T) == 0, "T can't be placed on page memory directly with required alignment.");

        if constexpr(is_huge)
        {
            static_assert(page_size == _2mb_ || page_size == _1gb_, "Only 2MB or 1GB hugepages are available");
        }

        if constexpr(!is_huge)
        {
            static_assert(page_size == _4kb_, "Only 4Kb pages are available");
        }
    }

    // STL-like interface

    [[nodiscard]] T* allocate(size_t n) noexcept
    {
        auto const pcount = calc_pages_count(n);
        return allocate_pages(pcount);
    }

    void deallocate(T* ptr, size_t n) noexcept
    {
        auto const pcount = calc_pages_count(n);
        deallocate_pages(ptr, pcount);
    }

    // IHFT-like interface

    [[nodiscard]] T* allocate_pages(size_t number_of_pages, bool touch_memory = true) noexcept
    {
        if (number_of_pages > 0)
        {
            #ifdef __APPLE__
            constexpr int MAP_HUGETLB_MARK = 0;
            constexpr int MAP_HUGE_PAGE_SIZE_MARK = 0;
            #else
            constexpr int MAP_HUGETLB_MARK = is_huge ? MAP_HUGETLB : 0;
            // Explicitly request the huge page size, otherwise the kernel uses the system default size.
            #if defined(MAP_HUGE_2MB) && defined(MAP_HUGE_1GB)
            constexpr int MAP_HUGE_PAGE_SIZE_MARK = is_huge ? ((page_size == _2mb_) ? MAP_HUGE_2MB : MAP_HUGE_1GB) : 0;
            #else
            constexpr int MAP_HUGE_PAGE_SIZE_MARK = 0;
            #endif
            #endif

            auto p = mmap(
                nullptr, number_of_pages * page_size, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB_MARK | MAP_HUGE_PAGE_SIZE_MARK, -1, 0);

            // mmap reports a failure with MAP_FAILED ((void*)-1), not nullptr.
            if (p == MAP_FAILED)
            {
                return nullptr;
            }

            if (touch_memory)
            {
                auto ptr = reinterpret_cast<volatile char*>(p);
                for(size_t i = 0; i < number_of_pages; i++)
                {
                    *ptr = 0;
                    ptr += page_size;
                }
            }

            return reinterpret_cast<T*>(p);
        }
        else
        {
            return nullptr;
        }
    }

    void deallocate_pages(T* p, size_t number_of_pages) noexcept
    {
        if (p == nullptr)
        {
            return;
        }

        munmap(p, number_of_pages * page_size);
    }

private:
    static size_t calc_pages_count(size_t n) noexcept
    {
        auto const size = n * sizeof(T);
        auto const pcount = size / page_size + (size % page_size > 0 ? 1 : 0);
        return pcount;
    }
};

}
