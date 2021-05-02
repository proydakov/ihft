#include "catch2/catch.hpp"

#include <platform/platform.h>
#include <memory/huge_page_allocator.h>

using namespace ihft;

namespace
{
    template<typename allocator_t>
    class memory_cleaner
    {
    public:
        memory_cleaner(allocator_t& allocator, void* ptr, size_t count)
            : m_allocator(allocator), m_ptr(ptr), m_count(count)
        {
        }

        ~memory_cleaner()
        {
            m_allocator.deallocate_pages(m_ptr, m_count);
        }

    private:
        allocator_t& m_allocator;
        void* m_ptr;
        size_t m_count;
    };

    static_assert(sizeof(size_t) == sizeof(void*), "Please cleanup tests below");
}

TEST_CASE("check 1gb huge page")
{
    if (ihft::platform::total_1gb_hugepages() < 2)
    {
        std::cerr << "hugepages 1gb are unavailable\n";
        return;
    }

    using allocator_t = one_gb_huge_page_allocator;

    constexpr size_t count = 2;
    allocator_t allocator;

    auto page = allocator.allocate_pages(count);
    memory_cleaner cleaner(allocator, page, count);

    std::cout << "1GB hugepage ptr: " << page << std::endl;

    REQUIRE( page != nullptr );

    REQUIRE( ((size_t)(page) & (allocator_t::huge_page_size - 1)) == 0 );
}

TEST_CASE("check 2mb huge page")
{
    if (ihft::platform::total_2mb_hugepages() < 2)
    {
        std::cerr << "hugepages 2mb are unavailable\n";
        return;
    }

    using allocator_t = two_mb_huge_page_allocator;

    constexpr size_t count = 2;
    allocator_t allocator;

    auto page = allocator.allocate_pages(count);
    memory_cleaner cleaner(allocator, page, count);

    std::cout << "2MB hugepage ptr: " << page << std::endl;

    REQUIRE( page != nullptr );

    REQUIRE( ((size_t)(page) & (allocator_t::huge_page_size - 1)) == 0 );
}
