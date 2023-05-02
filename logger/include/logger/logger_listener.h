#pragma once

#include <string_view>

namespace ihft::logger
{

class logger_listener
{
public:
    virtual ~logger_listener() noexcept;

    virtual void notify(std::string_view) = 0;
    virtual void flush() = 0;
};

}
