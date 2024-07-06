#include <catch2/catch_test_macros.hpp>

#include <logger/logger_event.h>

#include <sstream>

using namespace ihft::logger;

TEST_CASE("plain by value")
{
    logger_event event("(Args: {}, {}, '{}')", long{1024}, float{3.14f}, char{'A'});

    std::ostringstream sstream;
    event.print_args_to(sstream);

    REQUIRE(sstream.str() == "(Args: 1024, 3.14, 'A')");
}

TEST_CASE("plain by reference")
{
    auto ilong = unsigned{512};
    auto iflot = double{1.57f};
    auto ichar = char{'c'};

    logger_event event("(Args: {}, {}, '{}')", ilong, iflot, ichar);

    std::ostringstream sstream;
    event.print_args_to(sstream);

    REQUIRE(sstream.str() == "(Args: 512, 1.57, 'c')");
}

TEST_CASE("plain by const reference")
{
    auto const ilong = int{256};
    auto const iflot = double{7.77};
    auto const ichar = char{'Y'};

    logger_event event("(Args: {}, {}, '{}')", ilong, iflot, ichar);

    std::ostringstream sstream;
    event.print_args_to(sstream);

    REQUIRE(sstream.str() == "(Args: 256, 7.77, 'Y')");
}

TEST_CASE("string by value")
{
    logger_event event("(Args: {}, {}, {})", std::string{"IHFT"}, std::string{"C++"}, std::string_view{"zone"});

    std::ostringstream sstream;
    event.print_args_to(sstream);

    REQUIRE(sstream.str() == "(Args: IHFT, C++, zone)");
}

TEST_CASE("string by reference")
{
    std::string str1{"hello"};
    std::string str2{"world"};
    std::string_view view{"C++"};

    logger_event event("(Args: {}, {}, {})", str1, str2, view);

    std::ostringstream sstream;
    event.print_args_to(sstream);

    REQUIRE(sstream.str() == "(Args: hello, world, C++)");
}

TEST_CASE("string by const reference")
{
    std::string const str1{"Alice's"};
    std::string const str2{"Adventures"};
    std::string_view const view1{"in"};
    std::string_view const view2{"Wonderland"};

    logger_event event("(Args: {}, {}, {}, {})", str1, str2, view1, view2);

    std::ostringstream sstream;
    event.print_args_to(sstream);

    REQUIRE(sstream.str() == "(Args: Alice's, Adventures, in, Wonderland)");
}

TEST_CASE("char array by value")
{
    logger_event event("(Args: {}, {}, {})", "ARRAY", "is", "HERE");

    std::ostringstream sstream;
    event.print_args_to(sstream);

    REQUIRE(sstream.str() == "(Args: ARRAY, is, HERE)");
}

TEST_CASE("char array by reference")
{
    char arr1[4] = "C++";
    char arr2[3] = "20";

    logger_event event("(Args: {}, {})", arr1, arr2);

    std::ostringstream sstream;
    event.print_args_to(sstream);

    REQUIRE(sstream.str() == "(Args: C++, 20)");
}

TEST_CASE("char array by const reference")
{
    const char arr1[5] = "TEST";
    const char arr2[10] = "OPERATION";

    logger_event event("(Args: {}, {})", arr1, arr2);

    std::ostringstream sstream;
    event.print_args_to(sstream);

    REQUIRE(sstream.str() == "(Args: TEST, OPERATION)");
}

TEST_CASE("nullptr_t")
{
    logger_event event("({})", nullptr);

    std::ostringstream sstream;
    event.print_args_to(sstream);

    auto const str = sstream.str();
    REQUIRE((str == "(0)" or str == "(0x0)" or str == "((nil))"));
}

TEST_CASE("pointer")
{
    void* ptr = nullptr;

    logger_event event("({})", ptr);

    std::ostringstream sstream;
    event.print_args_to(sstream);

    auto const str = sstream.str();
    REQUIRE((str == "(0)" or str == "(0x0)" or str == "((nil))"));
}

TEST_CASE("const pointer")
{
    const void* ptr = nullptr;

    logger_event event("({})", ptr);

    std::ostringstream sstream;
    event.print_args_to(sstream);

    auto const str = sstream.str();
    REQUIRE((str == "(0)" or str == "(0x0)" or str == "((nil))"));
}
