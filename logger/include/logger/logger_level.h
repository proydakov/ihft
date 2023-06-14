#pragma once

#include <iosfwd>

namespace ihft::logger
{
    enum class log_level
    {
        _NONE_,
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    std::ostream& operator<<(std::ostream&, log_level);
}
