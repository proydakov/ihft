#include <catch2/catch.hpp>

#include <platform/platform.h>
#include <memory/page_allocator.h>
#include <memory/huge_page_allocator.h>

#include <limits>
#include <iostream>

using namespace ihft::memory;

namespace
{
    template<typename allocator_t>
    class memory_cleaner
    {
    public:
        memory_cleaner(allocator_t& allocator, std::byte* ptr, size_t count) noexcept
            : m_allocator(allocator), m_ptr(ptr), m_count(count)
        {
        }

        ~memory_cleaner() noexcept
        {
            m_allocator.deallocate_pages(m_ptr, m_count);
        }

    private:
        allocator_t& m_allocator;
        std::byte* m_ptr;
        size_t m_count;
    };

    template<typename allocator_t>
    void test_impl(unsigned total_pages, const char * const comment)
    {
        if (total_pages < 2)
        {
            std::cerr << comment << " are unavailable\n";
            return;
        }

        constexpr size_t count = 2;
        allocator_t allocator;

        auto page = allocator.allocate_pages(count);
        memory_cleaner cleaner(allocator, page, count);

        std::cout << comment << " ptr: " << page << std::endl;

        REQUIRE( page != nullptr );

        static_assert(sizeof(size_t) == sizeof(void*), "Please cleanup tests below");
        REQUIRE( ((size_t)(page) & (allocator_t::page_size - 1)) == 0 );
    }
}

TEST_CASE("check 4kb pages")
{
    test_impl<four_4b_page_allocator<>>(std::numeric_limits<unsigned>::max(), "pages 4kb");
}

#ifdef __linux__

TEST_CASE("check 1gb hugepages")
{
    test_impl<one_gb_huge_page_allocator<>>(ihft::platform::trait::total_1gb_hugepages(), "hugepages 1GB");
}

TEST_CASE("check 2mb hugepages")
{
    test_impl<one_gb_huge_page_allocator<>>(ihft::platform::trait::total_2mb_hugepages(), "hugepages 2mb");
}

#endif
