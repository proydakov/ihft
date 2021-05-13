#define TOML_EXCEPTIONS 0

#include <toml++/toml.h>

#include <fstream>
#include <iostream>
#include <string_view>

const char * const TOML = R"(
[library]
name = "toml++"
authors = ["Mark Gillard <mark.gillard@outlook.com.au>"]
cities = ["上海", "北京", "深圳"]
bogatyrs = ["Добрыня Никитич", "Илья Муромец", "Алёша Попович"]

[dependencies]
cpp = 17
)";

const char * const FNAME = "configuration.toml";

int main()
{
    {
        std::ofstream output( FNAME );
        output << TOML;
    }

    auto config = toml::parse_file( FNAME );

    // get key-value pairs
    std::string_view library_name = config["library"]["name"].value_or(std::string_view(""));
    if (config["library"]["authors"].is_array())
    {
        std::cout << "len: " << config["library"]["authors"].as_array()->size() << "\n";
    }
    std::string_view library_author = config["library"]["authors"][0].value_or(std::string_view(""));
    int64_t depends_on_cpp_version = config["dependencies"]["cpp"].value_or(0);

    std::cout << "library_name: " << library_name << "\n";
    std::cout << "library_author: " << library_author << "\n";
    std::cout << "depends_on_cpp_version: " << depends_on_cpp_version << "\n";

    std::cout << "\nRANGLE LOOP:\n\n";

    // iterate & visit over the data
    for (auto const& [k, v] : config)
    {
        v.visit([](auto& node) noexcept
        {
            std::cout << node << "\n";
        });
    }

    std::cout << "\nTOML:\n\n";

    // re-serialize as TOML
    std::cout << config << "\n";

    return 0;
}
