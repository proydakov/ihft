#include "catch2/catch.hpp"

#include <string>

namespace
{
    static int g_call_allocate = 0;
    static int g_call_deallocate = 0;

    template<typename T>
    class test_allocator
    {
    public:
        using value_type = T;

        T* allocate(std::size_t size)
        {
            g_call_allocate++;
            auto bytes = size * sizeof(T);
            auto ptr = reinterpret_cast<T*>(malloc(bytes));
            std::cout << "allocate: " << ptr << " size: " <<size << std::endl;
            return ptr;
        }

        void deallocate(T* ptr, std::size_t size)
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
    REQUIRE( g_call_allocate == 0);
    REQUIRE( g_call_deallocate == 0);

    {
        std::cout << "sizeof: " << sizeof(istring) << std::endl;

        constexpr int SSO_BUFFER_SIZE = 22;

        istring text;
        for(int i = 0; i < SSO_BUFFER_SIZE; i++)
        {
            text.push_back(static_cast<char>('a' + i));
        }
    }

    REQUIRE( g_call_allocate == 0);
    REQUIRE( g_call_deallocate == 0);
}
