#include <catch2/catch.hpp>

#include <misc/config_helper.h>
#include <types/temp_file.h>

#include <map>
#include <fstream>

namespace
{

const char * const VALID_TOML_DOC =
R"(str = "hello world"

[dependencies]
lang = "C++"
version = 20
compiler = "clang"
operation_system = "linux"
native = true
)";

const char * const INVALID_TOML_DOC =
R"([dependencies]
operation_system = "lin
valid = fal
)";

const char * const VALID_ENUMERATE_BOOLEAN =
R"([brands]
apple = true
nokia = false
samsung = true
)";

const char * const VALID_ENUMERATE_INTEGER =
R"([cities]
Moscow = 1147
Shanghai = 751
)";

const char * const VALID_ENUMERATE_STRING =
R"([operations]
next = "+1"
prev = "-1"
)";

}

using namespace ihft::misc;
using namespace ihft::types;

TEST_CASE("SUCCESS")
{
    temp_file file("test_misc_success", VALID_TOML_DOC);

    auto result = config_helper::parse( file.fpath() );

    REQUIRE( result.succeeded() );

    auto const config(std::move(result).value());

    REQUIRE( config.get_boolean("dependencies", "native") == true );

    REQUIRE( config.get_integer("dependencies", "version") == 20 );

    REQUIRE( config.get_string("dependencies", "operation_system") == "linux" );

    REQUIRE( config.get_string("dependencies", "compiler") == "clang" );

    REQUIRE( config.get_string("dependencies", "lang") == "C++" );
}

TEST_CASE("PROBLEM")
{
    temp_file file("test_misc_problem", INVALID_TOML_DOC);

    auto config = config_helper::parse( file.fpath() );

    REQUIRE( config.failed() );
}

TEST_CASE("ENUMERATE_BOOLEAN")
{
    temp_file file("test_misc_enumerate_boolean", VALID_ENUMERATE_BOOLEAN);

    auto result = config_helper::parse( file.fpath() );

    REQUIRE( result.succeeded() );

    auto const& config = result.value();

    std::map<std::string, bool> map;

    config.enumerate_boolean("brands", [&](std::string_view key, bool value) mutable {
        map[std::string(key)] = value;
    });

    std::map<std::string, bool> const etalon({{"apple", true}, {"nokia", false}, {"samsung", true}});

    REQUIRE( map == etalon );
}

TEST_CASE("ENUMERATE_INTEGER")
{
    temp_file file("test_misc_enumerate_integer", VALID_ENUMERATE_INTEGER);

    auto result = config_helper::parse( file.fpath() );

    REQUIRE( result.succeeded() );

    auto const& config = result.value();

    std::map<std::string, std::int64_t> map;

    config.enumerate_integer("cities", [&](std::string_view key, std::int64_t value) mutable {
        map[std::string(key)] = value;
    });

    std::map<std::string, std::int64_t> const etalon({{"Moscow", 1147}, {"Shanghai", 751}});

    REQUIRE( map == etalon );
}

TEST_CASE("ENUMERATE_STRING")
{
    temp_file file("test_misc_enumerate_string", VALID_ENUMERATE_STRING);

    auto result = config_helper::parse( file.fpath() );

    REQUIRE( result.succeeded() );

    auto const& config = result.value();

    std::map<std::string_view, std::string_view> map;

    config.enumerate_string("operations", [&](std::string_view key, std::string_view value) mutable {
        map[key] = value;
    });

    std::map<std::string_view, std::string_view> const etalon({{"next", "+1"}, {"prev", "-1"}});

    REQUIRE( map == etalon );
}
