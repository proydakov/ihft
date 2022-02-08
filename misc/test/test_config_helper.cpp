#include "catch2/catch.hpp"

#include <misc/config_helper.h>
#include <types/temp_file.h>

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
