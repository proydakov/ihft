#include "toml_doc.h"

#define TOML_EXCEPTIONS 0
#include <toml++/toml.h>

#include <iomanip>
#include <fstream>
#include <iostream>
#include <string_view>

const char * const FNAME = "configuration.toml";

int main()
{
    {
        std::ofstream output( FNAME );
        output << VALID_TOML_DOC;
    }

    auto result = toml::parse_file( FNAME );

    if (result.failed())
    {
        std::cerr << result.error() << "\n";
        return 1;
    }

    auto const& config = result.table();

    std::cout << "source: " <<  config.source() << "\n";

    std::cout << "sizeof(toml::parse_result): " << sizeof(config) << "\n";
    std::cout << "alignof(toml::parse_result): " << alignof(decltype(config)) << "\n";

    std::cout << "sizeof(toml::table): " << sizeof(toml::table) << "\n";
    std::cout << "alignof(toml::table): " << alignof(toml::table) << "\n";

    std::cout << "\nINFO:\n\n";
    std::cout << "size: " << config.size() << "\n";
    std::cout << "empty: " << std::boolalpha << config.empty() << "\n";

    std::cout << "\nDATA:\n\n";

    // get key-value pairs
    std::string_view library_name = config["library"]["name"].value_or(std::string_view(""));
    if (config["library"]["authors"].is_array())
    {
        std::cout << "len: " << config["library"]["authors"].as_array()->size() << "\n";
    }
    std::string_view library_author = config["library"]["authors"][0].value_or(std::string_view(""));
    int64_t depends_on_cpp_version = config["dependencies"]["version"].value_or(0);

    std::cout << "library_name: " << library_name << "\n";
    std::cout << "library_author: " << library_author << "\n";
    std::cout << "depends_on_cpp_version: " << depends_on_cpp_version << "\n";

    std::cout << "\nRANGLE LOOP:\n\n";

    // iterate & visit over the data
    for(auto const& [k, v] : config)
    {
        std::cout << "key: '" << k << "'\n";
        v.visit([](auto& node) noexcept
        {
            std::cout << node << "\n";
        });
        std::cout << '\n';
    }

    std::cout << "\nTOML:\n\n";

    // re-serialize as TOML
    std::cout << config << "\n";

    return 0;
}
