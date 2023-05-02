#include <logger/private/default_logger_listener.h>

#include <iostream>

namespace ihft::logger::impl
{
    void default_logger_listener::notify(std::string_view view)
    {
        std::cout << view << "\n";
    }

    void default_logger_listener::flush()
    {
        std::cout.flush();
    }
}
