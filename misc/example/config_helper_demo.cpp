#include "toml_doc.h"

#include <misc/config_helper.h>

#include <fstream>
#include <iostream>
#include <string_view>

using namespace ihft:: misc;

const char * const FNAME = "table.toml";

int main()
{
    {
        std::ofstream output( FNAME );
        output << VALID_TOML_DOC;
    }

    std::cout << "parsing\n";

    auto config = config_helper::parse( FNAME );

    std::cout << "parsed\n";

    if (config.failed())
    {
        std::cerr << config.error() << "\n";
        return 1;
    }

    std::cout << "\nTOML parsing looks good:\n\n";

    // re-serialize as TOML
    std::cout << config << "\n";

    return 1;
}
