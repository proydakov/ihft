#include "catch2/catch.hpp"

#include <string>
#include <iostream>

namespace
{
    static int g_call_allocate = 0;
    static int g_call_deallocate = 0;

    template<typename T>
    class test_allocator
    {
    public:
        using value_type = T;

        [[nodiscard]] T* allocate(std::size_t size) noexcept
        {
            g_call_allocate++;
            auto bytes = size * sizeof(T);
            auto ptr = reinterpret_cast<T*>(malloc(bytes));
            std::cout << "allocate: " << ptr << " size: " <<size << std::endl;
            return ptr;
        }

        void deallocate(T* ptr, std::size_t size) noexcept
        {
            std::cout << "deallocate: " << ptr << " size: " << size << std::endl;
            g_call_deallocate++;
            std::free(ptr);
        }

        std::size_t id{};
    };
}

using istring = std::basic_string<char, std::char_traits<char>, test_allocator<char>>;

TEST_CASE("SSO")
{
    REQUIRE(g_call_allocate == 0);
    REQUIRE(g_call_deallocate == 0);

    {
        std::cout << "sizeof: " << sizeof(istring) << std::endl;

#if defined (__clang__)
        constexpr int SSO_BUFFER_SIZE = 22;
#elif defined (__GNUC__)
        constexpr int SSO_BUFFER_SIZE = 15;
#else
#   error "Unsupported compiler !!!"
#endif

        istring text;
        for(int i = 0; i < SSO_BUFFER_SIZE; i++)
        {
            text.push_back(static_cast<char>('a' + i));
        }
    }

    REQUIRE(g_call_allocate == 0);
    REQUIRE(g_call_deallocate == 0);
}
