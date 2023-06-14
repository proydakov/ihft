#include <logger/logger_level.h>

#include <ostream>

namespace ihft::logger
{
    std::ostream& operator<<(std::ostream& os, log_level level)
    {
        switch(level)
        {
            case log_level::_NONE_:
            os << "NONE";
            break;

            case log_level::DEBUG:
            os << "DEBUG";
            break;

            case log_level::INFO:
            os << "INFO";
            break;

            case log_level::WARN:
            os << "WARN";
            break;

            case log_level::ERROR:
            os << "ERROR";
            break;
        }

        return os;
    }
}
