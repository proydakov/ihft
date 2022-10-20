#include <catch2/catch.hpp>

#include <string_view>

struct source_info
{
    std::string_view file{};
    std::string_view func{};
    unsigned line{};
};

source_info source_log(std::string_view file, std::string_view func, unsigned line)
{
    return {file, func, line};
}

consteval std::string_view prepare_file_path(std::string_view rawfile)
{
    size_t const pos = rawfile.rfind("/");
    if (pos == std::string_view::npos)
    {
        return rawfile;
    }
    else
    {
        return rawfile.substr(pos + 1, rawfile.length() - pos - 1);
    }
}

#define LOG( ) source_log( prepare_file_path(__FILE__), __PRETTY_FUNCTION__, __LINE__ )

// Unit tests

void source_info1()
{
    auto const info = LOG( );

    REQUIRE(info.file == "test_source_info.cpp");
    REQUIRE(info.func == "void source_info1()");
    REQUIRE(info.line == 36u);
}

TEST_CASE("source info 1")
{
    source_info1();
}

struct Holder
{
    static void source_info2()
    {
        auto const info = LOG( );

        REQUIRE(info.file == "test_source_info.cpp");
        REQUIRE(info.func == "static void Holder::source_info2()");
        REQUIRE(info.line == 52u);
    }
};

TEST_CASE("source info 2")
{
    Holder::source_info2();
}

struct Helper
{
    void source_info3() const
    {
        auto const info = LOG( );

        REQUIRE(info.file == "test_source_info.cpp");
        REQUIRE(info.func == "void Helper::source_info3() const");
        REQUIRE(info.line == 69u);
    }
};

TEST_CASE("source info 3")
{
    Helper helper;

    helper.source_info3();
}

#undef LOG
