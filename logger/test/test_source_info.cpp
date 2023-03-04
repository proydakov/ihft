#include <catch2/catch.hpp>

#include <string_view>

struct source_info final
{
    std::string_view file{};
    std::string_view func{};
    unsigned line{};
};

consteval source_info source_log(std::string_view file, std::string_view func, unsigned line)
{
    return {file, func, line};
}

consteval std::string_view prepare_file_path(std::string_view rawfile)
{
    auto const pos = rawfile.rfind("/");
    if (std::string_view::npos == pos)
    {
        return rawfile;
    }
    else
    {
        return rawfile.substr(pos + 1, rawfile.length() - pos - 1);
    }
}

#define LOG_SOURCE_INFO( ) source_log( prepare_file_path(__FILE__), __PRETTY_FUNCTION__, __LINE__ )

// Unit tests

void source_info1()
{
    auto constexpr info = LOG_SOURCE_INFO( );

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
        auto constexpr info = LOG_SOURCE_INFO( );

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
        auto constexpr info = LOG_SOURCE_INFO( );

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

struct Telper
{
    template<typename T>
    void source_info4() const
    {
        auto constexpr info = LOG_SOURCE_INFO( );

        REQUIRE(info.file == "test_source_info.cpp");
        REQUIRE((info.func == "void Telper::source_info4() const [T = void *]" ||
                 info.func == "void Telper::source_info4() const [with T = void*]"
        ));
        REQUIRE(info.line == 89u);
    }
};

TEST_CASE("source info 4")
{
    Telper helper;

    helper.source_info4<void*>();
}

#undef LOG_SOURCE_INFO
