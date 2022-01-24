#include "catch2/catch.hpp"

#include "misc/config_helper.h"

#include <fstream>

namespace
{

class temp_file
{
public:
    temp_file(std::string fname, std::string_view content)
        : m_fname(std::move(fname))
    {
        std::ofstream output(m_fname);
        output << content;
    }

    ~temp_file()
    {
        std::remove(m_fname.c_str());
    }

    std::string_view fname() const noexcept
    {
        return m_fname;
    }

private:
    std::string m_fname;
};

const char * const VALID_TOML_DOC =
R"([library]
[dependencies]
lang = "C++"
version = 20
compiler = "clang"
operation_system = "linux"
native = true
)";

const char * const INVALID_TOML_DOC =
R"([library]
[dependencies]
operation_system = "lin
valid = fal
)";

}

using namespace ihft::misc;

TEST_CASE("SUCCESS")
{
    temp_file file("test_misc_success", VALID_TOML_DOC);

    auto config = config_helper::parse( file.fname() );

    REQUIRE( config.succeeded() );
}

TEST_CASE("PROBLEM")
{
    temp_file file("test_misc_problem", INVALID_TOML_DOC);

    auto config = config_helper::parse( file.fname() );

    REQUIRE( config.failed() );
}
