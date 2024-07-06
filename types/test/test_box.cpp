#include <catch2/catch_test_macros.hpp>

#include <types/box.h>

#include <iostream>

using namespace ihft::types;

class RAII final
{
public:
    RAII(long& counter) : m_counter(counter)
    {
        m_counter++;
    }

    ~RAII()
    {
        m_counter--;
    }

private:
    long& m_counter;
};

TEST_CASE("create and destroy")
{
    std::aligned_storage_t<sizeof(RAII), alignof(RAII)> memory;

    long counter{};

    REQUIRE(counter == 0);

    {
        box<RAII> box(std::construct_at(reinterpret_cast<RAII*>(&memory), counter));

        REQUIRE(counter == 1);
        REQUIRE(box.has_value());
        REQUIRE(std::addressof(static_cast<RAII const&>(box)) == reinterpret_cast<RAII*>(&memory));
    }

    REQUIRE(counter == 0);
}
