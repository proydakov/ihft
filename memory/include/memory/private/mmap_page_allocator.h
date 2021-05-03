#pragma once

#include <sys/mman.h>

namespace ihft
{

//
// Code below uses MAP_HUGETLB. More details here:
// https://man7.org/linux/man-pages/man2/mmap.2.html
//

template <unsigned psize, bool is_huge>
struct mmap_page_allocator
{
    static constexpr unsigned page_size = psize;

    mmap_page_allocator()
    {
        if constexpr(is_huge)
        {
            static_assert(page_size == (1u << 21u) || page_size == (1u << 30u),
                "Only 2MB or 1GB hugepages are available");
        }

        if constexpr(!is_huge)
        {
            static_assert(page_size == (1u << 12u),
                "Only 4Kb pages are available");
        }
    }

    // IHFT-like interface

    [[nodiscard]] void* allocate_pages(size_t number_of_pages, bool touch_memory = true) noexcept
    {
        #ifdef __APPLE__
        constexpr int MAP_HUGETLB_MARK = 0;
        #else
        constexpr int MAP_HUGETLB_MARK = is_huge ? MAP_HUGETLB : 0;
        #endif

        auto p = mmap(
            nullptr, number_of_pages * page_size, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB_MARK, -1, 0);

        if (touch_memory && p != nullptr)
        {
            auto ptr = reinterpret_cast<volatile char*>(p);
            for(size_t i = 0; i < number_of_pages; i++)
            {
                *ptr = 0;
                ptr += page_size;
            }
        }

        return p;
    }

    void deallocate_pages(void* p, size_t number_of_pages) noexcept
    {
        munmap(p, number_of_pages * page_size);
    }
};

}
