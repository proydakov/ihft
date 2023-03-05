#include <catch2/catch.hpp>

#include <logger/private/source_location.h>

// Unit tests

void source_info1()
{
    auto constexpr info = source_location_current();

    REQUIRE(info.file_name() == "test_source_location.cpp");
    REQUIRE(info.function_name() == "void source_info1()");
    REQUIRE(info.line() == 9u);
}

TEST_CASE("source info 1")
{
    source_info1();
}

struct Holder final
{
    static void source_info2()
    {
        auto constexpr info = source_location_current( );

        REQUIRE(info.file_name() == "test_source_location.cpp");
        REQUIRE(info.function_name() == "static void Holder::source_info2()");
        REQUIRE(info.line() == 25u);
    }
};

TEST_CASE("source info 2")
{
    Holder::source_info2();
}

struct Helper final
{
    void source_info3() const
    {
        auto constexpr info = source_location_current( );

        REQUIRE(info.file_name() == "test_source_location.cpp");
        REQUIRE(info.function_name() == "void Helper::source_info3() const");
        REQUIRE(info.line() == 42u);
    }
};

TEST_CASE("source info 3")
{
    Helper helper;
    helper.source_info3();
}

struct Telper final
{
    template<typename T>
    void source_info4() const
    {
        auto constexpr info = source_location_current( );

        REQUIRE(info.file_name() == "test_source_location.cpp");
        REQUIRE((info.function_name() == "void Telper::source_info4() const [T = void *]" ||
                 info.function_name() == "void Telper::source_info4() const [with T = void*]"
        ));

        REQUIRE(info.line() == 61u);
    }
};

TEST_CASE("source info 4")
{
    Telper helper;
    helper.source_info4<void*>();
}
