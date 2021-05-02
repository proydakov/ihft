#pragma once

#include <sys/mman.h>

namespace ihft
{

//
// Code below uses MAP_HUGETLB. More details here:
// https://man7.org/linux/man-pages/man2/mmap.2.html
//

template <unsigned hp_size>
struct huge_page_allocator
{
    static constexpr unsigned huge_page_size = hp_size;

    static_assert(huge_page_size == (1u << 21u) || huge_page_size == (1u << 30u), "Only 2MB or 1GB hugepages is available");

    // IHFT-like interface

    [[nodiscard]] void* allocate_pages(size_t number_of_pages, bool touch_memory = true) noexcept
    {
        #ifdef __APPLE__
        constexpr int MAP_HUGETLB = 0;
        #endif

        auto p = mmap(
            nullptr, number_of_pages * huge_page_size, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);

        if (touch_memory && p != nullptr)
        {
            auto ptr = reinterpret_cast<volatile char*>(p);
            for(size_t i = 0; i < number_of_pages; i++)
            {
                *ptr = 0;
                ptr += huge_page_size;
            }
        }

        return p;
    }

    void deallocate_pages(void* p, size_t number_of_pages) noexcept
    {
        munmap(p, number_of_pages * huge_page_size);
    }
};

using two_mb_huge_page_allocator = huge_page_allocator<1u << 21u>;

using one_gb_huge_page_allocator = huge_page_allocator<1u << 30u>;

}
